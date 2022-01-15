// [03/12/2021 MRB]

#pragma once

#include "Core/Messaging/Event/bcEventListenerHandle.h"
#include "Platform/bcEvent.h"
#include "Game/System/Input/bcChasingCamera.h"
#include "App/SampleImp/XBot/bcXBotActorController.h"
#include "App/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_input_system;
		class bc_network_system;
		class bc_skinned_mesh_component;
		class bc_rigid_controller_component;
		class bc_human_ragdoll_component;
	}
	
	class BC_DLL bc_xbot_player_actor_controller : public bc_xbot_actor_controller
	{
	public:
		bc_xbot_player_actor_controller() noexcept;

		bc_xbot_player_actor_controller(bc_xbot_player_actor_controller&&) noexcept;
		
		~bc_xbot_player_actor_controller() override;

		bc_xbot_player_actor_controller& operator=(bc_xbot_player_actor_controller&&) noexcept;

		void enable_ragdoll(core::bc_string_view p_body_part_force, const core::bc_vector3f& p_force) noexcept;

	protected:
		void initialize(const game::bc_actor_component_initialize_context& p_context) override;

		void load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context) override;

		void load_replicated_network_instance(const game::bc_actor_component_network_load_context& p_context) override;

		void write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context) override;

		void write_replicated_network_instance(const game::bc_actor_component_network_write_context& p_context) override;

		void added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene) override;

		void update_origin_instance(const game::bc_actor_component_update_content& p_context) override;

		void update_replicated_instance(const game::bc_actor_component_update_content& p_context) override;

		void removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene) override;

		void handle_event(const game::bc_actor_component_event_context& p_context) override;

	private:
		void _on_event(core::bci_event& p_event) noexcept;

		void _on_pointing(const platform::bc_app_event_pointing& p_pointing_event) noexcept;

		void _on_key(const platform::bc_app_event_key& p_key_event) noexcept;

		void _start_grenade_throw(const bcCHAR* p_entity);
		
		void _attach_weapon(const bcCHAR* p_entity);

		void _detach_weapon();

		void _shoot_weapon();

		void throw_grenade(game::bc_actor& p_grenade) noexcept override final;

		virtual bool can_look() noexcept;

		virtual bool can_move() noexcept;

		virtual bool can_throw_grenade(core::bc_string_view p_entity) noexcept;

		virtual bool can_attach_weapon(core::bc_string_view p_entity) noexcept;

		virtual bool can_shoot_weapon() noexcept;

		virtual void grenade_thrown(core::bc_string_view p_entity) noexcept;

		virtual void weapon_attached(game::bc_actor& p_weapon) noexcept;

		virtual void weapon_shoot(game::bc_actor& p_weapon) noexcept;

		core::bc_event_listener_handle m_key_listener_handle;
		core::bc_event_listener_handle m_pointing_listener_handle;
		game::bc_input_system* m_input_system;
		game::bc_network_system* m_network_system;
		game::bc_chasing_camera* m_camera;

		bcFLOAT m_camera_y_offset;
		bcFLOAT m_camera_z_offset;
		bcFLOAT m_camera_look_at_offset;
		bcINT32 m_pointing_delta_x;
		bcINT32 m_pointing_last_x;
		bool m_forward_pressed;
		bool m_backward_pressed;
		bool m_right_pressed;
		bool m_left_pressed;
		bool m_walk_pressed;
		bool m_grenade_pressed;
		bcFLOAT m_grenade_throw_passed_time;

		const bcCHAR* m_rifle_name;
		const bcCHAR* m_grenade_name;
		const bcCHAR* m_threw_grenade_name;
		const bcCHAR* m_smoke_grenade_name;
		const bcCHAR* m_threw_smoke_grenade_name;
		bcFLOAT m_grenade_throw_time;
		bcFLOAT m_grenade_throw_force;
		core::bc_velocity<bcFLOAT> m_weapon_shoot_velocity;
	};
}
