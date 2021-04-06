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
#include "Game/Object/Scene/ActorComponent/bcActorController.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Animation/Job/bcSamplingAnimationJob.h"
#include "Game/Object/Animation/Job/bcBlendingAnimationJob.h"
#include "Game/Object/Animation/Job/bcTwoBoneIKAnimationJob.h"
#include "BlackCat/SampleImp/ActorController/bcXBotWeapon.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_physics_system;
		class bc_skeleton_animation;
		class bc_skinned_mesh_component;
	}
	
	class BC_DLL bc_xbot_controller : public game::bci_actor_controller, protected physics::bci_ccontroller_hit_callback
	{
		friend class bc_xbot_update_animation_job;
		
	public:
		bc_xbot_controller() noexcept;

		bc_xbot_controller(bc_xbot_controller&&) = default;
		
		virtual ~bc_xbot_controller() override = default;

		bc_xbot_controller& operator=(bc_xbot_controller&&) = default;
		
		void initialize(const game::bc_actor_component_initialize_context& p_context) override;
		
		void added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene) override;
		
		void update(const game::bc_actor_component_update_content& p_context) override;

		void removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene) override;
		
		void handle_event(const game::bc_actor_component_event_context& p_context) override;

	protected:
		game::bc_scene* get_scene() noexcept;

		game::bc_actor& get_actor() noexcept;

		bcFLOAT get_bound_box_max_side_length() const noexcept;
		
		const core::bc_vector3f& get_local_origin() const noexcept;
		
		const core::bc_vector3f& get_local_forward() const noexcept;

		game::bci_animation_job* get_idle_animation() const noexcept;
		
		game::bci_animation_job* get_running_animation() const noexcept;

		game::bci_animation_job* get_rifle_idle_animation() const noexcept;

		game::bci_animation_job* get_rifle_running_animation() const noexcept;

		const core::bc_vector3f& get_position() const noexcept;
		
		const core::bc_vector3f& get_look_direction() const noexcept;
		
		const core::bc_vector3f& get_move_direction() const noexcept;
		
		bcFLOAT get_move_speed() const noexcept;
		
		void set_active_animation(game::bci_animation_job* p_animation) noexcept;

		void set_look_delta(bcINT32 p_x_change) noexcept;

		void set_move_forward(bool p_value) noexcept;

		void set_move_backward(bool p_value) noexcept;

		void set_move_right(bool p_value) noexcept;
		
		void set_move_left(bool p_value) noexcept;

		void set_walk(bool p_value) noexcept;

		void attach_weapon(game::bc_actor& p_weapon) noexcept;

		void detach_weapon() noexcept;

		void shoot_weapon() noexcept;
		
		void on_shape_hit(const physics::bc_ccontroller_shape_hit& p_hit) override;

		void on_ccontroller_hit(const physics::bc_ccontroller_controller_hit& p_hit) override;
		
	private:
		core::bc_shared_ptr<game::bci_animation_job> _create_idle_animation(const bcCHAR* p_idle_animation,
			const bcCHAR* p_left_turn_animation,
			const bcCHAR* p_right_turn_animation,
			const bcCHAR* p_weapon_shoot_animation,
			core::bc_shared_ptr<game::bc_sampling_animation_job>* p_idle_sample_job);

		core::bc_shared_ptr<game::bci_animation_job> _create_running_animation(core::bc_shared_ptr<game::bc_sampling_animation_job> p_idle_sample_job,
			const bcCHAR* p_walking_animation,
			const bcCHAR* p_walking_backward_animation,
			const bcCHAR* p_running_animation,
			const bcCHAR* p_running_backward_animation,
			const bcCHAR* p_weapon_shoot_animation,
			core::bc_shared_ptr<game::bc_sampling_animation_job>* p_walking_sample_job,
			core::bc_shared_ptr<game::bc_sampling_animation_job>* p_walking_backward_sample_job,
			core::bc_shared_ptr<game::bc_sampling_animation_job>* p_running_sample_job,
			core::bc_shared_ptr<game::bc_sampling_animation_job>* p_running_backward_sample_job);

		void _update_input();

		void _update_direction();

		void _update_active_animation();

		void _blend_idle_animation(game::bci_animation_job& p_idle_animation);

		void _blend_running_animation(game::bci_animation_job& p_running_animation);

		void _blend_weapon_shoot_animation();
		
		void _weapon_ik_animation(bc_xbot_weapon* p_weapon, game::bci_animation_job & p_main_animation);
		
		void _update_world_transform();

		void _update_weapon_transform();

		game::bc_physics_system* m_physics_system;
		game::bc_scene* m_scene;
		game::bc_actor m_actor;
		game::bc_skinned_mesh_component* m_skinned_component;
		physics::bc_ccontroller_ref m_px_controller;
		bcFLOAT m_bound_box_max_side_length;
		core::bc_vector<core::bc_string> m_upper_body_chain;
		core::bc_array<std::pair<bcUINT32, const bcCHAR*>, 3> m_main_hand_chain;
		core::bc_array<std::pair<bcUINT32, const bcCHAR*>, 3> m_second_hand_chain;
		std::pair<bcUINT32, const bcCHAR*> m_rifle_joint;
		core::bc_vector3f m_rifle_joint_offset;
		core::bc_vector3f m_local_origin;
		core::bc_vector3f m_local_forward;

		core::bc_shared_ptr<game::bc_sampling_animation_job> m_idle_sample_job;
		core::bc_shared_ptr<game::bc_sampling_animation_job> m_rifle_aiming_idle_sample_job;
		core::bc_shared_ptr<game::bci_animation_job> m_idle_job;
		core::bc_shared_ptr<game::bci_animation_job> m_running_job;
		core::bc_shared_ptr<game::bci_animation_job> m_rifle_idle_job;
		core::bc_shared_ptr<game::bci_animation_job> m_rifle_running_job;
		game::bci_animation_job* m_active_job;

		bcFLOAT m_look_speed;
		bcFLOAT m_run_speed;
		bcFLOAT m_walk_speed;
		bcFLOAT m_move_speed;

		core_platform::bc_clock::update_param m_clock;
		bcINT32 m_look_delta_x;
		bool m_forward_pressed;
		bool m_backward_pressed;
		bool m_right_pressed;
		bool m_left_pressed;
		bool m_walk_pressed;
		core::bc_velocity<bcFLOAT> m_look_velocity;
		core::bc_velocity<bcFLOAT> m_forward_velocity;
		core::bc_velocity<bcFLOAT> m_backward_velocity;
		core::bc_velocity<bcFLOAT> m_right_velocity;
		core::bc_velocity<bcFLOAT> m_left_velocity;
		core::bc_velocity<bcFLOAT> m_walk_velocity;

		core::bc_vector3f m_position;
		core::bc_vector3f m_look_direction;
		core::bc_vector3f m_move_direction;
		bcFLOAT m_move_amount;

		core::bc_nullable<bc_xbot_weapon> m_weapon;
		bcFLOAT m_weapon_shoot_time;
	};

	inline game::bc_scene* bc_xbot_controller::get_scene() noexcept
	{
		return m_scene;
	}
	
	inline game::bc_actor& bc_xbot_controller::get_actor() noexcept
	{
		return m_actor;
	}

	inline bcFLOAT bc_xbot_controller::get_bound_box_max_side_length() const noexcept
	{
		return m_bound_box_max_side_length;
	}
	
	inline const core::bc_vector3f& bc_xbot_controller::get_local_origin() const noexcept
	{
		return m_local_origin;
	}
	
	inline const core::bc_vector3f& bc_xbot_controller::get_local_forward() const noexcept
	{
		return m_local_forward;
	}

	inline game::bci_animation_job* bc_xbot_controller::get_idle_animation() const noexcept
	{
		return m_idle_job.get();
	}

	inline game::bci_animation_job* bc_xbot_controller::get_running_animation() const noexcept
	{
		return m_running_job.get();
	}

	inline game::bci_animation_job* bc_xbot_controller::get_rifle_idle_animation() const noexcept
	{
		return m_rifle_idle_job.get();
	}

	inline game::bci_animation_job* bc_xbot_controller::get_rifle_running_animation() const noexcept
	{
		return m_rifle_running_job.get();
	}
	
	inline const core::bc_vector3f& bc_xbot_controller::get_position() const noexcept
	{
		return m_position;
	}

	inline const core::bc_vector3f& bc_xbot_controller::get_look_direction() const noexcept
	{
		return m_look_direction;
	}

	inline const core::bc_vector3f& bc_xbot_controller::get_move_direction() const noexcept
	{
		return m_move_direction;
	}

	inline bcFLOAT bc_xbot_controller::get_move_speed() const noexcept
	{
		return m_move_amount;
	}
	
	inline void bc_xbot_controller::set_active_animation(game::bci_animation_job* p_animation) noexcept
	{
		m_active_job = p_animation;
	}
}