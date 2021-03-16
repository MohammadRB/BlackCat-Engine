// [03/12/2021 MRB]

#pragma once

#include "Core/Utility/bcVelocity.h"
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
			bc_xbot_camera_controller();

			bc_xbot_camera_controller(bc_xbot_camera_controller&&) noexcept;
			
			~bc_xbot_camera_controller() override;

			bc_xbot_camera_controller& operator=(bc_xbot_camera_controller&&) noexcept;
			
			void initialize(const bc_actor_component_initialize_context& p_context) override;
			
			void added_to_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene) override;
			
			void update(const bc_actor_component_update_content& p_context) override;
			
			void handle_event(const bc_actor_component_event_context& p_context) override;

		private:
			void _update_input(const core_platform::bc_clock::update_param& p_clock);
			
			bc_input_system* m_input_system;
			bc_chasing_camera* m_camera;

			bcFLOAT m_camera_y_offset;
			bcFLOAT m_camera_z_offset;
			bcFLOAT m_camera_look_at_offset;
			core::bc_vector3f m_position;
			core::bc_vector3f m_forward;

			core::bc_velocity<bcFLOAT> m_forward_velocity;
		};
	}	
}