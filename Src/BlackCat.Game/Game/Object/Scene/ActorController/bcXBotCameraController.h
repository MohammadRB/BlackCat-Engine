// [03/12/2021 MRB]

#pragma once

#include "Game/System/Input/bcChasingCamera.h"
#include "Game/Object/Scene/ActorController/bcXBotController.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_input_system;
		
		class BC_GAME_DLL bc_xbot_camera_controller : public bc_xbot_controller
		{
		public:
			~bc_xbot_camera_controller() override;
			
			void initialize(const bc_actor_component_initialize_context& p_context) override;
			
			void added_to_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene) override;
			
			void update(const bc_actor_component_update_content& p_context) override;
			
			void handle_event(const bc_actor_component_event_context& p_context) override;

		private:
			bc_input_system* m_input_system = nullptr;
			bc_chasing_camera* m_camera = nullptr;

			bcFLOAT m_camera_y_offset = 0;
			bcFLOAT m_camera_z_offset = 0;
			bcFLOAT m_camera_look_at_offset = 0;
			core::bc_vector3f m_position;
			core::bc_vector3f m_forward;
		};
	}	
}