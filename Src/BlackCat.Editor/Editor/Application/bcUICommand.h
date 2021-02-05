// [02/19/2017 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcString.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Utility/bcParameterPack.h"
#include "PhysicsImp/Collision/bcShapeQuery.h"
#include "PhysicsImp/Collision/bcSceneQuery.h"
#include "Game/System/Physics/bcPxWrap.h"

namespace black_cat
{
	namespace game
	{
		class bc_game_system;
	}

	namespace editor
	{
		class bc_form_main_menu;
		class bc_form_object_insert;
		class bc_form_object;
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
			bc_ui_command_update_context(const core_platform::bc_clock::update_param& p_clock, 
				game::bc_game_system& p_game_system, 
				bc_iui_command_state* p_state)
				: m_clock(p_clock),
				m_game_system(p_game_system),
				m_state(p_state),
				m_result()
			{
			}

			const core_platform::bc_clock::update_param& m_clock;
			game::bc_game_system& m_game_system;
			bc_iui_command_state* m_state;
			core::bc_any m_result;
		};

		class bc_ui_command_update_ui_context
		{
		public:
			bc_ui_command_update_ui_context(bc_form_object& p_form_object, 
				bc_form_object_insert& p_form_object_insert,
				bc_form_main_menu& p_form_main_menu)
				: m_form_object(p_form_object),
				m_form_object_insert(p_form_object_insert),
				m_form_main_menu(p_form_main_menu)
			{
			}

			bc_form_object& m_form_object;
			bc_form_object_insert& m_form_object_insert;
			bc_form_main_menu& m_form_main_menu;
		};

		class bc_ui_command_undo_context
		{
		public:
			bc_ui_command_undo_context(const core_platform::bc_clock::update_param& p_clock,
				game::bc_game_system& p_game_system,
				bc_iui_command_state* p_state)
				: m_clock(p_clock),
				m_game_system(p_game_system),
				m_state(p_state)
			{
			}

			const core_platform::bc_clock::update_param& m_clock;
			game::bc_game_system& m_game_system;
			bc_iui_command_state* m_state;
		};

		class bc_iui_command
		{
		public:
			using state = bc_iui_command_state;
			using state_ptr = core::bc_unique_ptr<bc_iui_command_state>;
			using state_context = bc_ui_command_state_context;
			using update_context = bc_ui_command_update_context;
			using update_ui_context = bc_ui_command_update_ui_context;
			using undo_context = bc_ui_command_undo_context;

		public:
			virtual ~bc_iui_command() = default;

			virtual core::bc_string title() const = 0;

			virtual bool is_reversible() const;

			/**
			 * \brief Create a state object that will hold required data for executing an instance of this command.
			 * This state will be stored and passed to the update method of command every time a new instance of 
			 * command is executing.
			 * This method will be called from engine main thread
			 * \param p_context 
			 * \return 
			 */
			virtual state_ptr create_state(state_context& p_context) const;

			/**
			 * \brief Update command logic.
			 * This method will be called from engine main thread.
			 * \param p_context
			 * \return Return true if command must persist in next frame
			 */
			virtual bool update(update_context& p_context) = 0;

			/**
			 * \brief Update command logic in UI context.
			 * This method will be called from ui main thread.
			 */
			virtual void update_ui(update_ui_context& p_context);

		protected:
			physics::bc_ray get_pointer_ray(const update_context& p_context,
				bcUINT16 p_screen_width,
				bcUINT16 p_screen_height,
				bcUINT16 p_point_left,
				bcUINT16 p_point_top) const;

			bool query_ray_in_scene(const update_context& p_context,
				bcUINT16 p_screen_width, 
				bcUINT16 p_screen_height, 
				bcUINT16 p_point_left, 
				bcUINT16 p_point_top,
				game::bc_query_group p_query_group,
				physics::bc_query_flags p_flags,
				physics::bc_scene_ray_query_buffer& p_result) const;

			physics::bc_query_hit_type skinning_mesh_hit_check(const update_context& p_context,
				const physics::bc_ray& p_ray,
				physics::bc_scene_query_post_filter_data& p_filter_data) const;
		};

		class bc_iui_command_reversible : public bc_iui_command
		{
		public:
			virtual ~bc_iui_command_reversible() = default;

			bool is_reversible() const override final;

			/**
			 * \brief This method will be called from engine main thread.
			 * \param p_context 
			 */
			virtual void undo(undo_context& p_context) = 0;
		};

		inline bool bc_iui_command::is_reversible() const
		{
			return false;
		}
		
		inline bool bc_iui_command_reversible::is_reversible() const
		{
			return true;
		}
	}
}