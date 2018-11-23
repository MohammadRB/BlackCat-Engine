// [02/19/2017 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcQueue.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Utility/bcServiceManager.h"
#include "Editor/Application/bcUICommand.h"

namespace black_cat
{
	namespace core
	{
		class bc_content_stream_manager;
	}

	namespace game
	{
		class bc_game_system;
	}

	namespace editor
	{
		class bc_ui_command_service : public core::bc_iservice
		{
		private:
			BC_SERVICE(bc_ui_command_service)
			using command_hash_t = std::hash<core::bc_string>;
			using command_state_container = core::bc_unordered_map_program<command_hash_t::result_type, bc_iui_command::state_ptr>;

		public:
			bc_ui_command_service(core::bc_content_stream_manager& p_content_stream, game::bc_game_system& p_game_system);

			bc_ui_command_service(bc_ui_command_service&&) noexcept;

			~bc_ui_command_service();

			bc_ui_command_service& operator=(bc_ui_command_service&&) noexcept;

			void load_content();

			void unload_content();

			bcUINT32 command_count() const;

			template<typename T>
			void execute_command(T&& p_command);

			void undo();

			void update(core_platform::bc_clock::update_param p_elapsed) override;

			void update_ui(bc_iui_command::update_ui_context& p_context);

		protected:

		private:
			bc_iui_command::state* _get_command_state(const bc_iui_command& p_command);

			core::bc_content_stream_manager& m_content_stream;
			game::bc_game_system& m_game_system;

			mutable core_platform::bc_mutex m_commands_lock;
			core_platform::bc_clock::big_clock m_last_update_clock;
			core_platform::bc_clock::big_clock m_last_execute_clock;
			core::bc_queue<core::bc_shared_ptr<bc_iui_command>> m_commands_to_execute;
			core::bc_queue<core::bc_shared_ptr<bc_iui_command>> m_reversible_commands;
			core::bc_queue<core::bc_shared_ptr<bc_iui_command>> m_executed_commands;
			bcUINT32 m_commands_to_undo;

			command_state_container m_command_states;
		};

		template< typename T >
		void bc_ui_command_service::execute_command(T&& p_command)
		{
			static_assert(std::is_base_of_v<bc_iui_command, T>, "T must inherit from bc_iui_command");

			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> m_guard(m_commands_lock);
				
				if(m_last_execute_clock == m_last_update_clock) // Only allow one command per frame
				{
					return;
				}
				m_last_execute_clock = m_last_update_clock;

				auto l_command = core::bc_make_shared<T>(std::move(p_command));
				m_commands_to_execute.push(l_command);
				
				if(l_command->is_reversible())
				{
					m_reversible_commands.push(l_command);
				}
			}
		}
	}
}