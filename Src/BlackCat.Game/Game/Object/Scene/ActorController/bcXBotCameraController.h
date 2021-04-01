// [03/12/2021 MRB]

#pragma once

#include "Core/Messaging/Event/bcEventListenerHandle.h"
#include "Platform/bcEvent.h"
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
			bc_xbot_camera_controller() noexcept;

			bc_xbot_camera_controller(bc_xbot_camera_controller&&) noexcept;
			
			~bc_xbot_camera_controller() override;

			bc_xbot_camera_controller& operator=(bc_xbot_camera_controller&&) noexcept;
			
			void initialize(const bc_actor_component_initialize_context& p_context) override;
			
			void added_to_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene) override;
			
			void update(const bc_actor_component_update_content& p_context) override;

			void removed_from_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene) override;
			
			void handle_event(const bc_actor_component_event_context& p_context) override;
			
		private:
			bool _on_event(core::bci_event& p_event) noexcept;

			bool _on_pointing(platform::bc_app_event_pointing& p_pointing_event) noexcept;

			bool _on_key(platform::bc_app_event_key& p_key_event) noexcept;

			void _attach_weapon(const bcCHAR* p_entity);

			void _detach_weapon(const bcCHAR* p_detached_entity);
			
			core::bc_event_listener_handle m_key_listener_handle;
			core::bc_event_listener_handle m_pointing_listener_handle;
			bc_input_system* m_input_system;
			bc_chasing_camera* m_camera;

			bcFLOAT m_camera_y_offset;
			bcFLOAT m_camera_z_offset;
			bcFLOAT m_camera_look_at_offset;
			bcINT32 m_pointing_delta_x;
			bcINT32 m_pointing_last_x;

			const bcCHAR* m_rifle_name;
			const bcCHAR* m_detached_rifle_name;
			bc_actor m_current_weapon;
			core_platform::bc_clock::small_delta_time m_weapon_shoot_time;
		};
	}	
}