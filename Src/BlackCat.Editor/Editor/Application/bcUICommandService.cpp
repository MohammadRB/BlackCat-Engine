// [02/19/2017 MRB]

#include "Editor/EditorPCH.h"

#include "Core/Concurrency/bcConcurrency.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Game/System/bcGameSystem.h"
#include "Game/bcEvent.h"
#include "Editor/Application/bcUICommandService.h"
#include "PlatformImp/bcIDELogger.h"

namespace black_cat
{
	namespace editor
	{
		_bc_ui_command_entry::_bc_ui_command_entry(core::bc_unique_ptr<bci_ui_command> p_command)
			: m_command(std::move(p_command)),
			m_task_link(core::bc_delegate< core::bc_any() >(*this, &_bc_ui_command_entry::_execute_task)),
			m_update_context(nullptr),
			m_command_result(false)
		{
		}

		core::bc_task<core::bc_any> _bc_ui_command_entry::get_task()
		{
			return m_task_link.get_task();
		}

		bci_ui_command& _bc_ui_command_entry::get_command()
		{
			return *m_command;
		}

		bool _bc_ui_command_entry::command_update(bci_ui_command::update_context& p_context)
		{
			m_update_context = &p_context;
			m_task_link(core::bc_concurrency::current_thread_id());
			return m_command_result;
		}

		void _bc_ui_command_entry::command_update_ui(bci_ui_command::update_ui_context& p_context)
		{
			m_command->update_ui(p_context);
		}

		void _bc_ui_command_entry::command_undo(bci_ui_command::undo_context& p_context)
		{
			static_cast<bci_ui_command_reversible&>(*m_command).undo(p_context);
		}

		core::bc_any _bc_ui_command_entry::_execute_task()
		{
			m_command_result = m_command->update(*m_update_context);
			return m_update_context->m_result;
		}

		bc_ui_command_service::bc_ui_command_service(core::bc_content_stream_manager& p_content_stream, core::bc_event_manager& p_event_manager, game::bc_game_system& p_game_system)
			: m_content_stream(p_content_stream),
			m_game_system(p_game_system),
			m_last_update_clock(0),
			m_last_execute_clock(0),
			m_commands_to_undo(0),
			m_editor_mode(false)
		{
			m_editor_mode_event_handle = p_event_manager.register_event_listener<game::bc_event_editor_mode>
			(
				core::bc_event_manager::delegate_type(*this, &bc_ui_command_service::_event_handler)
			);
		}

		bc_ui_command_service::~bc_ui_command_service() = default;
		
		void bc_ui_command_service::load_content()
		{
			m_content_stream.read_stream_file(m_game_system.get_file_system().get_content_data_path(bcL("EditorContentStream.json")).c_str());
			m_content_stream.load_content_stream(core::bc_alloc_type::program, "editor_shaders");
		}

		void bc_ui_command_service::unload_content()
		{
			m_content_stream.unload_content_stream("editor_shaders");
		}

		bcUINT32 bc_ui_command_service::reversible_command_count() const
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
		
		void bc_ui_command_service::update(const core_platform::bc_clock::update_param& p_elapsed)
		{
			{
				core_platform::bc_mutex_guard l_guard(m_commands_lock);
				m_last_update_clock = p_elapsed.m_total_elapsed;

				while (!m_commands_to_execute.empty())
				{
					_bc_ui_command_entry& l_command_entry = *m_commands_to_execute.front();
					bci_ui_command::update_context l_context
					(
						p_elapsed, 
						m_game_system, 
						_get_command_state(l_command_entry.get_command())
					);

					if (!l_command_entry.command_update(l_context))
					{
						m_executed_commands.push(m_commands_to_execute.front());
						m_commands_to_execute.pop();
					}
				}
				
				if(m_commands_to_undo)
				{
					_bc_ui_command_entry& l_command_entry = *m_reversible_commands.front();
					bci_ui_command::undo_context l_context
					(
						p_elapsed,
						m_game_system,
						_get_command_state(l_command_entry.get_command())
					);
					l_command_entry.command_undo(l_context);

					m_reversible_commands.pop();
					m_commands_to_undo--;
				}
			}
		}

		void bc_ui_command_service::update_ui(bci_ui_command::update_ui_context& p_context)
		{
			{
				core_platform::bc_mutex_guard l_guard(m_commands_lock);

				while(!m_executed_commands.empty())
				{
					_bc_ui_command_entry& l_command_entry = *m_executed_commands.front();
					l_command_entry.command_update_ui(p_context);
					m_executed_commands.pop();
				}
			}
		}

		bci_ui_command::state* bc_ui_command_service::_get_command_state(const bci_ui_command& p_command)
		{
			auto l_command_hash = command_hash_t()(p_command.title());
			auto l_command_state = m_command_states.find(l_command_hash);

			if (l_command_state == std::cend(m_command_states))
			{
				bci_ui_command::state_context l_context(m_content_stream, m_game_system);
				bci_ui_command::state_ptr l_state = p_command.create_state(l_context);

				l_command_state = m_command_states.insert(command_state_container::value_type(l_command_hash, std::move(l_state))).first;
			}

			return l_command_state->second.get();
		}

		bool bc_ui_command_service::_event_handler(core::bci_event& p_event)
		{
			auto* l_editor_mode_event = core::bci_message::as<game::bc_event_editor_mode>(p_event);
			if (l_editor_mode_event)
			{
				m_editor_mode = l_editor_mode_event->get_editor_mode();
			}

			return false;
		}
	}
}