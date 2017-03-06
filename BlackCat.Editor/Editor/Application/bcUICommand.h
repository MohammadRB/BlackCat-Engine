// [02/19/2017 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcString.h"
#include "Core/File/bcContentStreamManager.h"
#include "Game/System/bcGameSystem.h"

namespace black_cat
{
	namespace editor
	{
		class bc_ui_command_service;

		class bc_iui_command_state
		{
		public:
			virtual ~bc_iui_command_state() = default;
		};

		class bc_ui_command_state_context
		{
		public:
			explicit bc_ui_command_state_context(const core::bc_content_stream_manager& p_content_stream,
				game::bc_game_system& p_game_system)
				: m_content_stream(p_content_stream),
				m_game_system(p_game_system)
			{
			}

			const core::bc_content_stream_manager& m_content_stream;
			game::bc_game_system& m_game_system;
		};

		class bc_ui_command_update_context
		{
		public:
			bc_ui_command_update_context(const core_platform::bc_clock::update_param& p_elapsed, 
				game::bc_game_system& p_game_system, 
				bc_iui_command_state* p_state)
				: m_elapsed(p_elapsed),
				m_game_system(p_game_system),
				m_state(p_state)
			{
			}

			const core_platform::bc_clock::update_param& m_elapsed;
			game::bc_game_system& m_game_system;
			bc_iui_command_state* m_state;
		};

		class bc_iui_commnad
		{
		public:
			using state = bc_iui_command_state;
			using state_ptr = core::bc_unique_ptr<bc_iui_command_state>;
			using state_context = bc_ui_command_state_context;
			using update_context = bc_ui_command_update_context;

		public:
			virtual ~bc_iui_commnad() = default;

			virtual core::bc_string title() const = 0;

			/**
			 * \brief Create a state object that will hold required data for executing an instance of this command.
			 * This states will be stored and passed to the update method of it's command every time a new instance
			 * of command is executing.
			 * This method will be called from engine main thread
			 * \param p_context 
			 * \return 
			 */
			virtual state_ptr create_state(state_context& p_context) const = 0;

			/**
			 * \brief Update command logic.
			 * This method will be called from engine main thread.
			 * \param p_context
			 * \return Return true if command must persist in next frame
			 */
			virtual bool update(update_context& p_context) = 0;
		};

		class bc_iui_command_reversable : public bc_iui_commnad
		{
		public:
			virtual ~bc_iui_command_reversable() = default;

			/**
			 * \brief This method will be called from engine main thread.
			 * \param p_context 
			 */
			virtual void rollback(update_context& p_context);
		};
	}
}