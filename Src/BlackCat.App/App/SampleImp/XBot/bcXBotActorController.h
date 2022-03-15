// [01/04/2021 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcArray.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Utility/bcVelocity.h"
#include "Core/Utility/bcNullable.h"
#include "PhysicsImp/Fundation/bcCController.h"
#include "PhysicsImp/Fundation/bcCControllerSimulationCallback.h"
#include "Game/Object/Scene/ActorComponent/bcActorNetworkController.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "App/SampleImp/XBot/bcXBotWeapon.h"
#include "App/SampleImp/XBot/bcXBotStateMachine.h"
#include "App/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_physics_system;
		class bc_skeleton_animation;
		class bc_skinned_mesh_component;
		class bc_rigid_controller_component;
		class bc_human_ragdoll_component;
	}

	struct bc_xbot_velocity
	{
		bcINT32 m_look_side;
		bcFLOAT m_look_velocity;
		bcFLOAT m_forward_velocity;
		bcFLOAT m_backward_velocity;
		bcFLOAT m_right_velocity;
		bcFLOAT m_left_velocity;
		bcFLOAT m_walk_velocity;
	};
	
	struct bc_xbot_input_update_context
	{
		const platform::bc_clock::update_param& m_clock;
		bcINT32 m_look_delta_x;
		bool m_forward_pressed;
		bool m_backward_pressed;
		bool m_right_pressed;
		bool m_left_pressed;
		bool m_walk_pressed;
	};

	struct bc_xbot_input_update_context1
	{
		const platform::bc_clock::update_param& m_clock;
		core::bc_vector3f m_position;
		core::bc_vector3f m_look_direction;
		bcINT32 m_look_side;
		bcFLOAT m_look_velocity;
		bool m_forward_pressed;
		bool m_backward_pressed;
		bool m_right_pressed;
		bool m_left_pressed;
		bool m_walk_pressed;
	};
	
	class BC_DLL bc_xbot_actor_controller : public game::bc_actor_network_controller, protected physics::bci_ccontroller_hit_callback
	{
	public:
		~bc_xbot_actor_controller() override = default;

		void update_attachment_transforms();
		
	protected:
		bc_xbot_actor_controller() noexcept;

		bc_xbot_actor_controller(bc_xbot_actor_controller&&) noexcept = default;

		bc_xbot_actor_controller& operator=(bc_xbot_actor_controller&&) noexcept = default;

		game::bc_scene* get_scene() noexcept;

		game::bc_actor& get_actor() noexcept;

		bcFLOAT get_bound_box_max_side_length() const noexcept;
		
		const core::bc_vector3f& get_local_origin() const noexcept;
		
		const core::bc_vector3f& get_local_forward() const noexcept;

		const core::bc_vector3f& get_position() const noexcept;
		
		const core::bc_vector3f& get_look_direction() const noexcept;
		
		const core::bc_vector3f& get_move_direction() const noexcept;

		bc_xbot_velocity get_velocity() const noexcept;
		
		bcFLOAT get_move_speed() const noexcept;

		bc_xbot_weapon* get_weapon() noexcept;

		bool get_ragdoll_enabled() const noexcept;

		void initialize(const game::bc_actor_component_initialize_context& p_context) override;

		void added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene) override;

		void update(const bc_xbot_input_update_context& p_context);

		void update(const bc_xbot_input_update_context1& p_context);

		void removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene) override;

		void handle_event(const game::bc_actor_component_event_context& p_context) override;

		physics::bc_ccontroller_ref create_px_controller();
		
		bool start_grenade_throw(const bcCHAR* p_entity_name) noexcept;
		
		void attach_weapon(game::bc_actor& p_weapon) noexcept;

		void detach_weapon() noexcept;

		bool shoot_weapon(game::bc_network_client_id p_player_id = game::bc_actor::invalid_id) noexcept;

		void enable_ragdoll();

		void enable_ragdoll(core::bc_string_view p_body_part_force, const core::bc_vector3f& p_force);

		void disable_ragdoll();
		
	private:
		void on_shape_hit(const physics::bc_ccontroller_shape_hit& p_hit) override;

		void on_ccontroller_hit(const physics::bc_ccontroller_controller_hit& p_hit) override;

		virtual void throw_grenade(game::bc_actor& p_grenade) noexcept = 0;

		core::bc_shared_ptr<game::bci_animation_job> _create_animation_pipeline(const bcCHAR* p_idle_animation,
			const bcCHAR* p_idle_left_turn_animation,
			const bcCHAR* p_idle_right_turn_animation,
			const bcCHAR* p_weapon_idle_animation,
			const bcCHAR* p_weapon_idle_left_turn_animation,
			const bcCHAR* p_weapon_idle_right_turn_animation,
			const bcCHAR* p_walking_animation,
			const bcCHAR* p_walking_backward_animation,
			const bcCHAR* p_running_animation,
			const bcCHAR* p_running_backward_animation,
			const bcCHAR* p_weapon_walking_animation,
			const bcCHAR* p_weapon_walking_backward_animation,
			const bcCHAR* p_weapon_running_animation,
			const bcCHAR* p_weapon_running_backward_animation,
			const bcCHAR* p_grenade_throw_animation,
			const bcCHAR* p_weapon_shoot_animation);
		
		void _update_px_move(const platform::bc_clock::update_param& p_clock, const core::bc_vector3f& p_move_vector);
		
		void _update_px_position(const core::bc_vector3f& p_position);
		
		void _update_world_transform();

		core::bc_matrix4f _calculate_weapon_aim_transform();

		core::bc_matrix4f _calculate_attachment_transform(bcUINT32 p_attached_node_index, 
			const core::bc_vector3f& p_attached_node_local_up,
			const core::bc_vector3f& p_attached_node_local_forward,
			const core::bc_vector3f& p_attached_node_local_offset,
			const core::bc_vector3f& p_attachment_local_up,
			const core::bc_vector3f& p_attachment_local_forward,
			const core::bc_vector3f& p_attachment_local_offset);

		physics::bc_material m_px_controller_material;
		game::bc_physics_system* m_physics_system;
		game::bc_scene* m_scene;
		game::bc_actor m_actor;
		game::bc_skinned_mesh_component* m_skinned_mesh_component;
		game::bc_rigid_controller_component* m_rigid_controller_component;
		game::bc_human_ragdoll_component* m_human_ragdoll_component;

		core::bc_vector3f m_local_origin;
		core::bc_vector3f m_local_forward;
		core::bc_vector<core::bc_string> m_upper_body_chain;
		core::bc_array<std::pair<bcUINT32, const bcCHAR*>, 3> m_right_hand_chain;
		core::bc_array<std::pair<bcUINT32, const bcCHAR*>, 3> m_left_hand_chain;
		core::bc_vector3f m_right_hand_weapon_up;
		core::bc_vector3f m_right_hand_weapon_forward;
		core::bc_vector3f m_right_hand_attachment_offset;
		core::bc_vector3f m_left_hand_weapon_up;
		core::bc_vector3f m_left_hand_weapon_forward;
		core::bc_vector3f m_left_hand_attachment_offset;
		std::pair<bcUINT32, const bcCHAR*> m_rifle_joint;
		core::bc_vector3f m_rifle_joint_offset;
		bcFLOAT m_grenade_anim_attach_time;
		bcFLOAT m_grenade_anim_throw_time;

		bcFLOAT m_bound_box_max_side_length;
		bcINT32 m_look_delta_x;
		core::bc_velocity<bcFLOAT> m_look_velocity;
		core::bc_velocity<bcFLOAT> m_forward_velocity;
		core::bc_velocity<bcFLOAT> m_backward_velocity;
		core::bc_velocity<bcFLOAT> m_right_velocity;
		core::bc_velocity<bcFLOAT> m_left_velocity;
		core::bc_velocity<bcFLOAT> m_walk_velocity;
		core::bc_vector3f m_position;
		bool m_ragdoll_enabled;
		
		core::bc_unique_ptr<bc_xbot_state_machine> m_state_machine;
		core::bc_shared_ptr<game::bci_animation_job> m_animation_pipeline;
		core::bc_nullable<bc_xbot_weapon> m_weapon;
		core::bc_nullable<bc_xbot_grenade> m_grenade;
	};

	inline game::bc_scene* bc_xbot_actor_controller::get_scene() noexcept
	{
		return m_scene;
	}
	
	inline game::bc_actor& bc_xbot_actor_controller::get_actor() noexcept
	{
		return m_actor;
	}

	inline bcFLOAT bc_xbot_actor_controller::get_bound_box_max_side_length() const noexcept
	{
		return m_bound_box_max_side_length;
	}
	
	inline const core::bc_vector3f& bc_xbot_actor_controller::get_local_origin() const noexcept
	{
		return m_local_origin;
	}
	
	inline const core::bc_vector3f& bc_xbot_actor_controller::get_local_forward() const noexcept
	{
		return m_local_forward;
	}

	inline const core::bc_vector3f& bc_xbot_actor_controller::get_position() const noexcept
	{
		return m_position;
	}

	inline const core::bc_vector3f& bc_xbot_actor_controller::get_look_direction() const noexcept
	{
		return m_state_machine->m_state.m_look_direction;
	}

	inline const core::bc_vector3f& bc_xbot_actor_controller::get_move_direction() const noexcept
	{
		return m_state_machine->m_state.m_move_direction;
	}

	inline bc_xbot_velocity bc_xbot_actor_controller::get_velocity() const noexcept
	{
		return bc_xbot_velocity
		{
			m_state_machine->m_state.m_look_side,
			m_look_velocity.get_value(),
			m_forward_velocity.get_value(),
			m_backward_velocity.get_value(),
			m_right_velocity.get_value(),
			m_left_velocity.get_value(),
			m_walk_velocity.get_value()
		};
	}

	inline bcFLOAT bc_xbot_actor_controller::get_move_speed() const noexcept
	{
		return m_state_machine->m_state.m_move_amount;
	}

	inline bc_xbot_weapon* bc_xbot_actor_controller::get_weapon() noexcept
	{
		return m_weapon.get();
	}

	inline bool bc_xbot_actor_controller::get_ragdoll_enabled() const noexcept
	{
		return m_ragdoll_enabled;
	}
}