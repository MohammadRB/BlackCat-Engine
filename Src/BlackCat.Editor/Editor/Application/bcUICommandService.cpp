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
			m_last_update_clock(0),
			m_last_execute_clock(0),
			m_commands_to_undo(0)
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
			m_commands_to_execute = std::move(p_other.m_commands_to_execute);
			m_reversible_commands = std::move(p_other.m_reversible_commands);
			m_executed_commands = std::move(p_other.m_executed_commands);
			m_commands_to_undo = p_other.m_commands_to_undo;
			m_command_states = std::move(p_other.m_command_states);

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
				core_platform::bc_lock_guard<core_platform::bc_mutex> l_guard(m_commands_lock);
				return m_reversible_commands.size();
			}
		}

		void bc_ui_command_service::undo()
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> l_guard(m_commands_lock);
				m_commands_to_undo++;
			}
		}
		
		void bc_ui_command_service::update(core_platform::bc_clock::update_param p_elapsed)
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> l_guard(m_commands_lock);
				m_last_update_clock = p_elapsed.m_total_elapsed;

				while (!m_commands_to_execute.empty())
				{
					bc_iui_command& l_command = *m_commands_to_execute.front();
					bc_iui_command::update_context l_context
					(
						p_elapsed, 
						m_game_system, 
						_get_command_state(l_command)
					);

					if (!l_command.update(l_context))
					{
						m_executed_commands.push(m_commands_to_execute.front());
						m_commands_to_execute.pop();
					}
				}
				
				if(m_commands_to_undo)
				{
					bc_iui_command_reversible& l_command = static_cast<bc_iui_command_reversible&>(*m_reversible_commands.front());
					bc_iui_command::update_context l_context
					(
						p_elapsed,
						m_game_system,
						_get_command_state(l_command)
					);
					l_command.rollback(l_context);

					m_reversible_commands.pop();
					m_commands_to_undo--;
				}
			}
		}

		void bc_ui_command_service::update_ui(bc_iui_command::update_ui_context& p_context)
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> l_guard(m_commands_lock);

				if(!m_executed_commands.empty())
				{
					bc_iui_command& l_command = *m_executed_commands.front();
					l_command.update_ui(p_context);
					m_executed_commands.pop();
				}
			}
		}

		bc_iui_command::state* bc_ui_command_service::_get_command_state(const bc_iui_command& p_command)
		{
			auto l_command_hash = command_hash_t()(p_command.title());
			auto l_command_state = m_command_states.find(l_command_hash);

			if (l_command_state == std::cend(m_command_states))
			{
				bc_iui_command::state_context l_context(m_content_stream, m_game_system);
				bc_iui_command::state_ptr l_state = p_command.create_state(l_context);

				l_command_state = m_command_states.insert(command_state_container::value_type(l_command_hash, std::move(l_state))).first;
			}

			return l_command_state->second.get();
		}
	}
}