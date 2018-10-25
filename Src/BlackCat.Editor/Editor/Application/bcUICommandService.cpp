// [02/19/2017 MRB]

#include "Editor/EditorPCH.h"

#include "Core/File/bcContentStreamManager.h"
#include "Game/System/bcGameSystem.h"
#include "Editor/Application/bcUICommandService.h"

namespace black_cat
{
	namespace editor
	{
		bc_ui_command_service::bc_ui_command_service(core::bc_content_stream_manager& p_content_stream, game::bc_game_system& p_game_system)
			: m_content_stream(p_content_stream),
			m_game_system(p_game_system),
			m_reversable_command_to_update(nullptr)
		{
		}

		bc_ui_command_service::bc_ui_command_service(bc_ui_command_service&& p_other) noexcept
			: m_content_stream(p_other.m_content_stream),
			m_game_system(p_other.m_game_system)
		{
			operator=(std::move(p_other));
		}

		bc_ui_command_service::~bc_ui_command_service() = default;

		bc_ui_command_service& bc_ui_command_service::operator=(bc_ui_command_service&& p_other) noexcept
		{
			m_command_to_update = std::move(p_other.m_command_to_update);
			m_reversable_command_to_update = p_other.m_reversable_command_to_update;
			m_reversable_commands = std::move(p_other.m_reversable_commands);

			return *this;
		}

		void bc_ui_command_service::load_content()
		{
			m_content_stream.read_stream_file(m_game_system.get_file_system().get_content_data_path(bcL("EditorContentStream.json")).c_str());
			m_content_stream.load_content_stream(core::bc_alloc_type::program, "editor_shaders");
		}

		void bc_ui_command_service::unload_content()
		{
			m_content_stream.unload_content_stream("editor_shaders");
		}

		bcUINT32 bc_ui_command_service::command_count() const
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> m_guard(m_commands_lock);

				return m_reversable_commands.size();
			}
		}

		void bc_ui_command_service::undo()
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> m_guard(m_commands_lock);

				if(m_reversable_commands.empty())
				{
					return;
				}

				m_reversable_command_to_rollback = std::move(m_reversable_commands.front());
				m_reversable_commands.pop_back();
			}
		}

		void bc_ui_command_service::update(core_platform::bc_clock::update_param p_elapsed)
		{
			core_platform::bc_lock_guard<core_platform::bc_mutex> m_guard(m_commands_lock);
			{
				if (m_command_to_update)
				{
					bc_iui_commnad::update_context l_context
					(
						p_elapsed, 
						m_game_system, 
						_get_command_state(m_command_to_update.get())
					);

					if (!m_command_to_update->update(l_context))
					{
						m_command_to_update.reset();
					}
				}
				else if (m_reversable_command_to_update)
				{
					bc_iui_commnad::update_context l_context
					(
						p_elapsed,
						m_game_system, 
						_get_command_state(m_reversable_command_to_update)
					);

					if (!m_reversable_command_to_update->update(l_context))
					{
						m_reversable_command_to_update = nullptr;
					}
				}
				else if(m_reversable_command_to_rollback)
				{
					bc_iui_commnad::update_context l_context
					(
						p_elapsed,
						m_game_system,
						_get_command_state(m_reversable_command_to_rollback.get())
					);

					m_reversable_command_to_rollback->rollback(l_context);
				}
			}
		}

		void bc_ui_command_service::_execute_command(core::bc_unique_ptr<bc_iui_commnad> p_command, std::false_type)
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> m_guard(m_commands_lock);

				m_command_to_update = std::move(p_command);
			}
		}

		void bc_ui_command_service::_execute_command(core::bc_unique_ptr<bc_iui_command_reversable> p_command, std::true_type)
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> m_guard(m_commands_lock);

				m_reversable_commands.push_back(std::move(p_command));
				m_reversable_command_to_update = &*m_reversable_commands.front();
			}
		}

		bc_iui_commnad::state* bc_ui_command_service::_get_command_state(const bc_iui_commnad* p_command)
		{
			auto l_command_hash = command_hash_t()(p_command->title());
			auto l_command_state = m_command_states.find(l_command_hash);

			if (l_command_state == std::cend(m_command_states))
			{
				bc_iui_commnad::state_context l_context(m_content_stream, m_game_system);

				bc_iui_commnad::state_ptr l_state = p_command->create_state(l_context);

				l_command_state = m_command_states.insert(command_state_container::value_type(l_command_hash, std::move(l_state))).first;
			}

			return l_command_state->second.get();
		}
	}
}