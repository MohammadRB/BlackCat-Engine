// [19/02/2017 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcQueue.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Concurrency/bcTask.h"
#include "Core/Messaging/Event/bcEventListenerHandle.h"
#include "Core/Utility/bcParameterPack.h"
#include "Core/Utility/bcServiceManager.h"
#include "Editor/Application/bcUICommand.h"
#include "Editor/UICommand/bcEditorModeUICommand.h"
#include "PlatformImp/bcIDELogger.h"

namespace black_cat
{
	namespace core
	{
		class bc_content_stream_manager;
		class bc_event_manager;
	}

	namespace game
	{
		class bc_game_system;
	}

	namespace editor
	{
		class _bc_ui_command_entry
		{
		public:
			_bc_ui_command_entry(core::bc_unique_ptr<bci_ui_command> p_command);

			bci_ui_command& get_command();

			core::bc_task<core::bc_any> get_task();

			bool command_update(bci_ui_command::update_context& p_context);

			void command_update_ui(bci_ui_command::update_ui_context& p_context);

			void command_undo(bci_ui_command::undo_context& p_context);

		private:
			core::bc_any _execute_task();

			core::bc_unique_ptr<bci_ui_command> m_command;
			core::bc_task_link<core::bc_any> m_task_link;
			core::bc_any m_task_result;
		};

		class bc_ui_command_service : public core::bci_service
		{
			BC_SERVICE(uic_srv)
			
		private:
			using command_hash_t = std::hash<core::bc_string>;
			using command_state_container = core::bc_unordered_map_program<command_hash_t::result_type, bci_ui_command::state_ptr>;

		public:
			bc_ui_command_service(core::bc_content_stream_manager& p_content_stream, core::bc_event_manager& p_event_manager, game::bc_game_system& p_game_system);

			bc_ui_command_service(bc_ui_command_service&&) noexcept = delete;

			~bc_ui_command_service() override;

			bc_ui_command_service& operator=(bc_ui_command_service&&) noexcept = delete;

			void load_content();

			void unload_content();

			bcUINT32 reversible_command_count() const;

			template<typename T>
			core::bc_task<core::bc_any> queue_command(T&& p_command);

			void undo();

			void update(const platform::bc_clock::update_param& p_elapsed) override;

			void update_ui(bci_ui_command::update_ui_context& p_context);

		private:
			bci_ui_command::state* _get_command_state(const bci_ui_command& p_command);

			void _event_handler(core::bci_event& p_event);

			core::bc_content_stream_manager& m_content_stream;
			game::bc_game_system& m_game_system;

			core::bc_event_listener_handle m_editor_mode_event_handle;
			bool m_editor_mode;
			platform::bc_clock::big_clock m_last_update_clock;
			platform::bc_clock::big_clock m_last_command_clock;

			mutable platform::bc_mutex m_commands_lock;
			command_state_container m_command_states;
			core::bc_queue<core::bc_shared_ptr<_bc_ui_command_entry>> m_commands_to_execute;
			core::bc_queue<core::bc_shared_ptr<_bc_ui_command_entry>> m_executed_commands;
			core::bc_queue<core::bc_shared_ptr<_bc_ui_command_entry>> m_reversible_commands;
			bcUINT32 m_commands_to_undo;
		};

		template<typename T>
		core::bc_task<core::bc_any> bc_ui_command_service::queue_command(T&& p_command)
		{
			static_assert(std::is_base_of_v<bci_ui_command, T>, "T must inherit from bc_iui_command");

			// Only allow editor mode command when application is in game mode
			if(!m_editor_mode && !std::is_same_v<bc_editor_mode_ui_command, T>)
			{
				return core::bc_task<core::bc_any>();
			}
			
			{
				platform::bc_mutex_guard l_lock(m_commands_lock);
				
				if(m_last_command_clock == m_last_update_clock) // Only allow one command per frame
				{
					return core::bc_task<core::bc_any>();
				}
				m_last_command_clock = m_last_update_clock;

				auto l_command = core::bc_make_unique<T>(std::move(p_command));
				auto l_command_entry = core::bc_make_shared<_bc_ui_command_entry>(std::move(l_command));

				m_commands_to_execute.push(l_command_entry);
				
				if(l_command_entry->get_command().is_reversible())
				{
					m_reversible_commands.push(l_command_entry);
				}

				return l_command_entry->get_task();
			}
		}
	}
}