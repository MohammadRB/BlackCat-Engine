// [10/09/2021 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/Math/bcCoordinate.h"
#include "Core/Container/bcStateMachine.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Animation/bcAnimationJobHelper.h"
#include "Game/Object/Animation/Job/bcBlendingAnimationJob.h"
#include "Game/Object/Animation/Job/bcAdditiveBlendingAnimationJob.h"
#include "BlackCat/SampleImp/ActorController/bcXBotWeaponIKAnimationJob.h"

namespace black_cat
{
	class bc_xbot_state_machine;
	class bc_xbot_idle_state;
	class bc_xbot_running_state;
	class bc_xbot_rifle_idle_state;
	class bc_xbot_rifle_running_state;

	struct bc_xbot_state
	{
		bcFLOAT m_look_speed;
		bcFLOAT m_run_speed;
		bcFLOAT m_walk_speed;
		core::bc_vector3f m_look_direction;
		core::bc_vector3f m_move_direction;
		bcFLOAT m_move_speed;
		bcFLOAT m_move_amount;
		bcFLOAT m_weapon_shoot_time;
	};

	struct bc_xbot_state_update_params
	{
		const core_platform::bc_clock::update_param& m_clock;
		const core::bc_vector3f& m_position;
		bcINT32 m_look_delta_x;
		bcFLOAT m_look_velocity;
		bcFLOAT m_forward_velocity;
		bcFLOAT m_right_velocity;
		bcFLOAT m_left_velocity;
		bcFLOAT m_backward_velocity;
		bcFLOAT m_walk_velocity;
	};
	
	struct bc_xbot_update_event
	{
		const bc_xbot_state_update_params& m_update_params;
		bc_xbot_state& m_state;
	};

	struct bc_xbot_weapon_attach_event
	{
		bc_xbot_weapon& m_weapon;
		const bcCHAR* m_offset_joint;
		const core::bc_vector3f& m_offset;
	};

	struct bc_xbot_weapon_detach_event
	{
	};

	struct bc_xbot_weapon_shoot_event
	{
		bc_xbot_state& m_state;
	};

	template<class ...TEvents>
	class bc_xbot_base_state : public core::bc_state<bc_xbot_state_machine, TEvents...>
	{
	public:
		virtual game::bci_animation_job& get_animation() noexcept = 0;
	
	protected:
		void update_directions(bc_xbot_update_event& p_event) const noexcept
		{
			core::bc_matrix3f l_look_rotation;
			core::bc_vector3f l_right_vector;
			core::bc_vector3f l_left_vector;

			if constexpr (graphic::bc_render_api_info::use_left_handed())
			{
				l_look_rotation.rotation_y_lh(core::bc_to_radian
				(
					p_event.m_update_params.m_look_delta_x * 
					p_event.m_update_params.m_look_velocity * 
					p_event.m_state.m_look_speed * 
					p_event.m_update_params.m_clock.m_elapsed_second
				));
			}
			else
			{
				l_look_rotation.rotation_y_rh(core::bc_to_radian
				(
					p_event.m_update_params.m_look_delta_x * 
					p_event.m_update_params.m_look_velocity * 
					p_event.m_state.m_look_speed * 
					p_event.m_update_params.m_clock.m_elapsed_second
				));
			}

			p_event.m_state.m_look_direction = core::bc_vector3f::normalize(l_look_rotation * p_event.m_state.m_look_direction);

			if constexpr (graphic::bc_render_api_info::use_left_handed())
			{
				core::bc_matrix3f l_right_left_direction;

				l_right_left_direction.rotation_y_lh(core::bc_to_radian(p_event.m_update_params.m_right_velocity * 90));
				l_right_vector = core::bc_vector3f::normalize(l_right_left_direction * p_event.m_state.m_look_direction);

				l_right_left_direction.rotation_y_lh(core::bc_to_radian(p_event.m_update_params.m_left_velocity * -90));
				l_left_vector = core::bc_vector3f::normalize(l_right_left_direction * p_event.m_state.m_look_direction);
			}
			else
			{
				core::bc_matrix3f l_right_left_direction;

				l_right_left_direction.rotation_y_rh(core::bc_to_radian(p_event.m_update_params.m_right_velocity * 90));
				l_right_vector = core::bc_vector3f::normalize(l_right_left_direction * p_event.m_state.m_look_direction);

				l_right_left_direction.rotation_y_rh(core::bc_to_radian(p_event.m_update_params.m_left_velocity * -90));
				l_left_vector = core::bc_vector3f::normalize(l_right_left_direction * p_event.m_state.m_look_direction);
			}

			p_event.m_state.m_move_speed = (1 - p_event.m_update_params.m_walk_velocity) * p_event.m_state.m_run_speed + p_event.m_update_params.m_walk_velocity * p_event.m_state.m_walk_speed;
			p_event.m_state.m_move_direction = p_event.m_state.m_look_direction * p_event.m_update_params.m_forward_velocity +
				p_event.m_state.m_look_direction * -p_event.m_update_params.m_backward_velocity +
				l_right_vector * p_event.m_update_params.m_right_velocity +
				l_left_vector * p_event.m_update_params.m_left_velocity;
			p_event.m_state.m_move_amount = std::max
			(
				{
					p_event.m_update_params.m_forward_velocity,
					p_event.m_update_params.m_backward_velocity,
					p_event.m_update_params.m_right_velocity,
					p_event.m_update_params.m_left_velocity
				}
			) * p_event.m_state.m_move_speed;

			if (core::bc_vector3f::length_sq(p_event.m_state.m_move_direction) <= 0)
			{
				p_event.m_state.m_move_amount = 0;
			}

			if (p_event.m_state.m_move_amount <= 0)
			{
				p_event.m_state.m_move_direction = p_event.m_state.m_look_direction;
			}

			p_event.m_state.m_move_direction.normalize();

			// if two vectors are exactly opposite of each other 'rotation_between_two_vector' method in matrix3x3 will have undefined result
			if (p_event.m_state.m_move_direction.dot(p_event.m_state.m_look_direction) >= -1.f)
			{
				p_event.m_state.m_move_direction.x -= 0.01f;
				p_event.m_state.m_move_direction.normalize();
			}
		}

		void blend_idle_animation(bc_xbot_update_event& p_event, game::bci_animation_job& p_idle_animation)
		{
			const auto l_look_velocity = p_event.m_update_params.m_look_velocity;
			bcFLOAT l_weights[3] = { l_look_velocity, 1 - l_look_velocity, l_look_velocity };

			auto* l_blending_job = game::bc_animation_job_helper::find_job<game::bc_blending_animation_job>(static_cast<game::bc_sequence_animation_job&>(p_idle_animation));
			l_blending_job->set_weights(&l_weights[0]);

			if (l_look_velocity == 0.f)
			{
				bcFLOAT l_times[3] = { 0, -1, 0 };
				l_blending_job->set_local_times(&l_times[0]);
			}
			else
			{
				bcFLOAT l_times[3] = { -1, 0, -1 };
				l_blending_job->set_local_times(&l_times[0]);
			}

			auto* l_aim_job = game::bc_animation_job_helper::find_job<game::bc_aim_animation_job>(static_cast<game::bc_sequence_animation_job&>(p_idle_animation));
			l_aim_job->set_world_target(p_event.m_update_params.m_position + p_event.m_state.m_look_direction * 1000);
		}

		void blend_running_animation(bc_xbot_update_event& p_event, game::bci_animation_job& p_running_animation)
		{
			const auto l_run_weight = (p_event.m_state.m_move_speed - p_event.m_state.m_walk_speed) / (p_event.m_state.m_run_speed - p_event.m_state.m_walk_speed);
			const auto l_walk_weight = 1 - l_run_weight;
			const auto l_move_speed_normalize = p_event.m_state.m_move_amount / p_event.m_state.m_move_speed;
			bcFLOAT l_weights[] =
			{
				1 - l_move_speed_normalize,
				l_move_speed_normalize * l_walk_weight,
				l_move_speed_normalize * l_walk_weight,
				l_move_speed_normalize * l_run_weight,
				l_move_speed_normalize * l_run_weight,
			};

			const auto l_move_direction_dot = p_event.m_state.m_look_direction.dot(p_event.m_state.m_move_direction);
			if (l_move_direction_dot >= -0.01f)
			{
				l_weights[2] = 0;
				l_weights[4] = 0;
			}
			else
			{
				l_weights[1] = 0;
				l_weights[3] = 0;
			}

			auto* l_blending_job = game::bc_animation_job_helper::find_job<game::bc_blending_animation_job>(static_cast<game::bc_sequence_animation_job&>(p_running_animation));
			l_blending_job->set_weights(&l_weights[0]);

			auto* l_aim_job = game::bc_animation_job_helper::find_job<game::bc_aim_animation_job>(static_cast<game::bc_sequence_animation_job&>(p_running_animation));
			l_aim_job->set_world_target(p_event.m_update_params.m_position + p_event.m_state.m_look_direction * 1000);
		}

		void place_weapon_ik(bc_xbot_weapon& p_weapon, game::bci_animation_job& p_active_animation)
		{
			auto* l_weapon_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(static_cast<game::bc_sequence_animation_job&>(p_active_animation));
			if (!l_weapon_ik_job)
			{
				return;
			}

			l_weapon_ik_job->set_weapon(&p_weapon);
		}

		void blend_weapon_shoot_animation(bc_xbot_update_event& p_event, bc_xbot_weapon& p_weapon, game::bci_animation_job& p_active_animation)
		{
			auto* l_additive_blend_job = game::bc_animation_job_helper::find_job<game::bc_additive_blending_animation_job>(static_cast<game::bc_sequence_animation_job&>(p_active_animation));
			if (!l_additive_blend_job)
			{
				return;
			}

			auto* l_additive_layer = l_additive_blend_job->get_additive_layer().get();
			if (!l_additive_layer)
			{
				return;
			}

			if (p_event.m_state.m_weapon_shoot_time >= 0)
			{
				p_event.m_state.m_weapon_shoot_time += p_event.m_update_params.m_clock.m_elapsed_second;

				if (p_event.m_state.m_weapon_shoot_time >= p_weapon.m_rate_of_fire_seconds)
				{
					p_event.m_state.m_weapon_shoot_time = -1;
				}
			}

			// Increase weapon shoot impact in running mode
			l_additive_blend_job->set_weights(1, p_event.m_state.m_move_amount ? std::max((p_event.m_state.m_move_amount / p_event.m_state.m_run_speed) * 2, 1.4f) : 1);

			// Once weapon shoot animation stopped disable additive blend
			if (!l_additive_layer->get_enabled())
			{
				l_additive_layer->set_local_time(0);
				l_additive_blend_job->set_enabled(false);
			}
		}

		void shoot_weapon(bc_xbot_weapon_shoot_event& p_event, game::bci_animation_job& p_active_animation)
		{
			auto* l_additive_blend_job = game::bc_animation_job_helper::find_job<game::bc_additive_blending_animation_job>(static_cast<game::bc_sequence_animation_job&>(p_active_animation));
			if (!l_additive_blend_job || !l_additive_blend_job->get_additive_layer())
			{
				return;
			}

			if (p_event.m_state.m_weapon_shoot_time >= 0)
			{
				return;
			}

			p_event.m_state.m_weapon_shoot_time = 0;
			l_additive_blend_job->get_additive_layer()->set_local_time(0);
			l_additive_blend_job->get_additive_layer()->set_enabled(true);
			l_additive_blend_job->set_enabled(true);
		}
	};
	
	class bc_xbot_idle_state : public bc_xbot_base_state<bc_xbot_update_event, bc_xbot_weapon_attach_event>
	{
	public:
		explicit bc_xbot_idle_state(game::bci_animation_job& p_animation)
			: m_animation(&p_animation)
		{
		}

		game::bci_animation_job& get_animation() noexcept override
		{
			return *m_animation;
		}

	private:
		state_transition handle(bc_xbot_update_event& p_event) override
		{
			update_directions(p_event);
			
			if(p_event.m_state.m_move_amount > 0)
			{
				return state_transition::transfer_to<bc_xbot_running_state>();
			}

			return state_transition::empty();
		}

		state_transition handle(bc_xbot_weapon_attach_event& p_event) override
		{
			return state_transition::transfer_to<bc_xbot_rifle_idle_state>([&](bc_xbot_rifle_idle_state& p_state)
			{
				p_state.set_weapon(p_event.m_weapon, p_event.m_offset_joint, p_event.m_offset);
			});
		}
		
		game::bci_animation_job* m_animation;
	};

	class bc_xbot_running_state : public bc_xbot_base_state<bc_xbot_update_event, bc_xbot_weapon_attach_event>
	{
	public:
		explicit bc_xbot_running_state(game::bci_animation_job& p_animation)
			: m_animation(&p_animation)
		{
		}

		game::bci_animation_job& get_animation() noexcept override
		{
			return *m_animation;
		}

	private:
		state_transition handle(bc_xbot_update_event& p_event) override
		{
			update_directions(p_event);

			if (p_event.m_state.m_move_amount <= 0)
			{
				return state_transition::transfer_to<bc_xbot_idle_state>();
			}

			return state_transition::empty();
		}

		state_transition handle(bc_xbot_weapon_attach_event& p_event) override
		{
			return state_transition::transfer_to<bc_xbot_rifle_running_state>([&](bc_xbot_rifle_running_state& p_state)
			{
				p_state.set_weapon(p_event.m_weapon, p_event.m_offset_joint, p_event.m_offset);
			});
		}
		
		game::bci_animation_job* m_animation;
	};

	class bc_xbot_rifle_idle_state : public bc_xbot_base_state<bc_xbot_update_event, bc_xbot_weapon_shoot_event, bc_xbot_weapon_detach_event>
	{
	public:
		explicit bc_xbot_rifle_idle_state(game::bci_animation_job& p_animation)
			: m_animation(&p_animation)
		{
		}

		game::bci_animation_job& get_animation() noexcept override
		{
			return *m_animation;
		}

		void set_weapon(bc_xbot_weapon& p_weapon, const bcCHAR* p_offset_joint, const core::bc_vector3f& p_offset)
		{
			auto* l_rifle_idle_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(static_cast<game::bc_sequence_animation_job&>(*m_animation));
			
			l_rifle_idle_ik_job->set_offset_joint(p_offset_joint);
			l_rifle_idle_ik_job->set_offset(p_offset);
		}
		
	private:
		state_transition handle(bc_xbot_update_event& p_event) override
		{
			update_directions(p_event);

			if (p_event.m_state.m_move_amount > 0)
			{
				return state_transition::transfer_to<bc_xbot_rifle_running_state>();
			}

			return state_transition::empty();
		}

		state_transition handle(bc_xbot_weapon_shoot_event& p_event) override
		{
			shoot_weapon(p_event, *m_animation);
			return state_transition::empty();
		}
		
		state_transition handle(bc_xbot_weapon_detach_event& p_event) override
		{
			return state_transition::transfer_to<bc_xbot_idle_state>();
		}
		
		game::bci_animation_job* m_animation;		
	};

	class bc_xbot_rifle_running_state : public bc_xbot_base_state<bc_xbot_update_event, bc_xbot_weapon_shoot_event, bc_xbot_weapon_detach_event>
	{
	public:
		explicit bc_xbot_rifle_running_state(game::bci_animation_job& p_animation)
			: m_animation(&p_animation)
		{
		}

		game::bci_animation_job& get_animation() noexcept override
		{
			return *m_animation;
		}

		void set_weapon(bc_xbot_weapon& p_weapon, const bcCHAR* p_offset_joint, const core::bc_vector3f& p_offset)
		{
			auto* l_rifle_running_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(static_cast<game::bc_sequence_animation_job&>(*m_animation));

			l_rifle_running_ik_job->set_offset_joint(p_offset_joint);
			l_rifle_running_ik_job->set_offset(p_offset);
		}

	private:
		state_transition handle(bc_xbot_update_event& p_event) override
		{
			update_directions(p_event);

			if (p_event.m_state.m_move_amount <= 0)
			{
				return state_transition::transfer_to<bc_xbot_rifle_idle_state>();
			}

			return state_transition::empty();
		}

		state_transition handle(bc_xbot_weapon_shoot_event& p_event) override
		{
			shoot_weapon(p_event, *m_animation);
			return state_transition::empty();
		}
		
		state_transition handle(bc_xbot_weapon_detach_event& p_event) override
		{
			return state_transition::transfer_to<bc_xbot_running_state>();
		}

		game::bci_animation_job* m_animation;
	};

	class bc_xbot_state_machine : core::bc_state_machine
	<
		bc_xbot_idle_state,
		bc_xbot_rifle_idle_state,
		bc_xbot_running_state,
		bc_xbot_rifle_running_state
	>
	{
	public:
		bc_xbot_state_machine(const core::bc_vector3f& p_local_forward, 
			bcFLOAT p_look_speed, 
			bcFLOAT p_run_speed, 
			bcFLOAT p_walk_speed,
			game::bci_animation_job& p_idle_animation,
			game::bci_animation_job& p_running_animation,
			game::bci_animation_job& p_rifle_idle_animation,
			game::bci_animation_job& p_rifle_running_animation)
			: bc_state_machine
			(
				bc_xbot_idle_state(p_idle_animation),
				bc_xbot_rifle_idle_state(p_running_animation),
				bc_xbot_running_state(p_rifle_idle_animation),
				bc_xbot_rifle_running_state(p_rifle_running_animation)
			),
			m_state{p_look_speed, p_run_speed, p_walk_speed, p_local_forward, p_local_forward, 0, 0, 0}
		{
		}

		bc_xbot_state_machine(bc_xbot_state_machine&&) noexcept = default;

		~bc_xbot_state_machine() = default;

		bc_xbot_state_machine& operator=(bc_xbot_state_machine&&) noexcept = default;

		void update(const bc_xbot_state_update_params& p_update)
		{
			bc_xbot_update_event l_event{ p_update, m_state };
			process_event(l_event);
		}

		void attach_weapon(bc_xbot_weapon& p_weapon, const bcCHAR* p_offset_joint, const core::bc_vector3f& p_offset)
		{
			bc_xbot_weapon_attach_event l_event{ p_weapon, p_offset_joint, p_offset };
			process_event(l_event);
		}

		void detach_weapon()
		{
			bc_xbot_weapon_detach_event l_event{};
			process_event(l_event);
		}

		void shoot_event()
		{
			bc_xbot_weapon_shoot_event l_event{m_state};
			process_event(l_event);
		}
		
		bc_xbot_state m_state;
	};
}