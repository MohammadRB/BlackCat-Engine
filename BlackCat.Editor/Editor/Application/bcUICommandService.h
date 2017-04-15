// [02/19/2017 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
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
			using command_state_container = core::bc_unordered_map_program<command_hash_t::result_type, bc_iui_commnad::state_ptr>;

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

		protected:

		private:
			void _execute_command(core::bc_unique_ptr<bc_iui_commnad> p_command, std::false_type);
	
			void _execute_command(core::bc_unique_ptr<bc_iui_command_reversable> p_command, std::true_type);

			bc_iui_commnad::state* _get_command_state(const bc_iui_commnad* p_command);

			core::bc_content_stream_manager& m_content_stream;
			game::bc_game_system& m_game_system;

			mutable core_platform::bc_mutex m_commands_lock;
			core::bc_unique_ptr<bc_iui_commnad> m_command_to_update;
			bc_iui_command_reversable* m_reversable_command_to_update;
			core::bc_unique_ptr<bc_iui_command_reversable> m_reversable_command_to_rollback;
			core::bc_vector<core::bc_unique_ptr<bc_iui_command_reversable>> m_reversable_commands;

			command_state_container m_command_states;
		};

		template< typename T >
		void bc_ui_command_service::execute_command(T&& p_command)
		{
			static_assert(std::is_base_of_v<bc_iui_commnad, T>, "T must inherit from bc_iui_command");

			core::bc_unique_ptr<T> l_command = core::bc_make_unique<T>(std::move(p_command));

			_execute_command(std::move(l_command), std::is_base_of<bc_iui_command_reversable, T>::type());
		}
	}
}