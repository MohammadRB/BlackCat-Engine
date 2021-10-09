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
#include "BlackCat/SampleImp/ActorController/bcXBotWeaponIKAnimationJob.h"

namespace black_cat
{
	class bc_xbot_state_machine;
	class bc_xbot_idle_state;
	class bc_xbot_running_state;
	class bc_xbot_rifle_idle_state;
	class bc_xbot_rifle_running_state;
	
	struct bc_xbot_update_event
	{
		const core_platform::bc_clock::update_param& m_clock;
		bcFLOAT m_look_speed;
		bcFLOAT m_run_speed;
		bcFLOAT m_walk_speed;
		bcINT32 m_look_delta_x;
		bcFLOAT m_look_velocity;
		bcFLOAT m_forward_velocity;
		bcFLOAT m_right_velocity;
		bcFLOAT m_left_velocity;
		bcFLOAT m_backward_velocity;
		bcFLOAT m_walk_velocity;

		bcFLOAT& m_move_speed;
		core::bc_vector3f& m_look_direction;
		core::bc_vector3f& m_move_direction;
		bcFLOAT& m_move_amount;
	};

	struct bc_xbot_weapon_attach_event
	{
		bc_xbot_weapon m_weapon;
		const bcCHAR* m_offset_joint;
		const core::bc_vector3f& m_offset;
	};

	struct bc_xbot_weapon_detach_event
	{
	};
	
	struct bc_xbot_state_update
	{
		const core_platform::bc_clock::update_param& m_clock;
		bcINT32 m_look_delta_x;
		bcFLOAT m_look_velocity;
		bcFLOAT m_forward_velocity;
		bcFLOAT m_right_velocity;
		bcFLOAT m_left_velocity;
		bcFLOAT m_backward_velocity;
		bcFLOAT m_walk_velocity;
	};

	template<class ...TEvents>
	class bc_xbot_state : public core::bc_state<bc_xbot_state_machine, TEvents...>
	{
	public:
		virtual game::bci_animation_job& get_animation() noexcept = 0;
	
	protected:
		void update_directions(bc_xbot_update_event& p_update_event) const noexcept
		{
			core::bc_matrix3f l_look_rotation;
			core::bc_vector3f l_right_vector;
			core::bc_vector3f l_left_vector;

			if constexpr (graphic::bc_render_api_info::use_left_handed())
			{
				l_look_rotation.rotation_y_lh(core::bc_to_radian(p_update_event.m_look_delta_x * p_update_event.m_look_velocity * p_update_event.m_look_speed * p_update_event.m_clock.m_elapsed_second));
			}
			else
			{
				l_look_rotation.rotation_y_rh(core::bc_to_radian(p_update_event.m_look_delta_x * p_update_event.m_look_velocity * p_update_event.m_look_speed * p_update_event.m_clock.m_elapsed_second));
			}

			p_update_event.m_look_direction = core::bc_vector3f::normalize(l_look_rotation * p_update_event.m_look_direction);

			if constexpr (graphic::bc_render_api_info::use_left_handed())
			{
				core::bc_matrix3f l_right_left_direction;

				l_right_left_direction.rotation_y_lh(core::bc_to_radian(p_update_event.m_right_velocity * 90));
				l_right_vector = core::bc_vector3f::normalize(l_right_left_direction * p_update_event.m_look_direction);

				l_right_left_direction.rotation_y_lh(core::bc_to_radian(p_update_event.m_left_velocity * -90));
				l_left_vector = core::bc_vector3f::normalize(l_right_left_direction * p_update_event.m_look_direction);
			}
			else
			{
				core::bc_matrix3f l_right_left_direction;

				l_right_left_direction.rotation_y_rh(core::bc_to_radian(p_update_event.m_right_velocity * 90));
				l_right_vector = core::bc_vector3f::normalize(l_right_left_direction * p_update_event.m_look_direction);

				l_right_left_direction.rotation_y_rh(core::bc_to_radian(p_update_event.m_left_velocity * -90));
				l_left_vector = core::bc_vector3f::normalize(l_right_left_direction * p_update_event.m_look_direction);
			}

			p_update_event.m_move_speed = (1 - p_update_event.m_walk_velocity) * p_update_event.m_run_speed + p_update_event.m_walk_velocity * p_update_event.m_walk_speed;
			p_update_event.m_move_direction = p_update_event.m_look_direction * p_update_event.m_forward_velocity +
				p_update_event.m_look_direction * -p_update_event.m_backward_velocity +
				l_right_vector * p_update_event.m_right_velocity +
				l_left_vector * p_update_event.m_left_velocity;
			p_update_event.m_move_amount = std::max
			(
				{
					p_update_event.m_forward_velocity,
					p_update_event.m_backward_velocity,
					p_update_event.m_right_velocity,
					p_update_event.m_left_velocity
				}
			) * p_update_event.m_move_speed;

			if (core::bc_vector3f::length_sq(p_update_event.m_move_direction) <= 0)
			{
				p_update_event.m_move_amount = 0;
			}

			if (p_update_event.m_move_amount <= 0)
			{
				p_update_event.m_move_direction = p_update_event.m_look_direction;
			}

			p_update_event.m_move_direction.normalize();

			// if two vectors are exactly opposite of each other 'rotation_between_two_vector' method in matrix3x3 will have undefined result
			if (p_update_event.m_move_direction.dot(p_update_event.m_look_direction) >= -1.f)
			{
				p_update_event.m_move_direction.x -= 0.01f;
				p_update_event.m_move_direction.normalize();
			}
		}

		void blend_idle_animation(game::bci_animation_job& p_idle_animation)
		{
			
		}

		void blend_running_animation(game::bci_animation_job& p_running_animation)
		{
			
		}

		void place_weapon_ik(bc_xbot_weapon& p_weapon, game::bci_animation_job& p_main_animation)
		{
			
		}

		void blend_weapon_shoot_animation(bc_xbot_weapon& p_weapon)
		{
			
		}
	};
	
	class bc_xbot_idle_state : public bc_xbot_state<bc_xbot_update_event, bc_xbot_weapon_attach_event>
	{
	public:
		explicit bc_xbot_idle_state(game::bci_animation_job& p_animation_job)
			: m_animation_job(&p_animation_job)
		{
		}

		game::bci_animation_job& get_animation() noexcept override
		{
			return *m_animation_job;
		}

	private:
		state_transition handle(bc_xbot_update_event& p_event) override
		{
			update_directions(p_event);

			if(p_event.m_move_amount > 0)
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
		
		game::bci_animation_job* m_animation_job;
	};

	class bc_xbot_running_state : public bc_xbot_state<bc_xbot_update_event, bc_xbot_weapon_attach_event>
	{
	public:
		explicit bc_xbot_running_state(game::bci_animation_job& p_animation_job)
			: m_animation_job(&p_animation_job)
		{
		}

		game::bci_animation_job& get_animation() noexcept override
		{
			return *m_animation_job;
		}

	private:
		state_transition handle(bc_xbot_update_event& p_event) override
		{
			update_directions(p_event);

			if (p_event.m_move_amount <= 0)
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
		
		game::bci_animation_job* m_animation_job;
	};

	class bc_xbot_rifle_idle_state : public bc_xbot_state<bc_xbot_update_event, bc_xbot_weapon_detach_event>
	{
	public:
		explicit bc_xbot_rifle_idle_state(game::bci_animation_job& p_animation_job)
			: m_animation_job(&p_animation_job)
		{
		}

		game::bci_animation_job& get_animation() noexcept override
		{
			return *m_animation_job;
		}

		void set_weapon(bc_xbot_weapon& p_weapon, const bcCHAR* p_offset_joint, const core::bc_vector3f& p_offset)
		{
			auto* l_rifle_idle_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(static_cast<game::bc_sequence_animation_job&>(*m_animation_job));
			
			l_rifle_idle_ik_job->set_offset_joint(p_offset_joint);
			l_rifle_idle_ik_job->set_offset(p_offset);
		}
	
	private:
		state_transition handle(bc_xbot_update_event& p_event) override
		{
			update_directions(p_event);

			if (p_event.m_move_amount > 0)
			{
				return state_transition::transfer_to<bc_xbot_rifle_running_state>();
			}

			return state_transition::empty();
		}

		state_transition handle(bc_xbot_weapon_detach_event& p_event) override
		{
			return state_transition::transfer_to<bc_xbot_idle_state>();
		}
		
		game::bci_animation_job* m_animation_job;		
	};

	class bc_xbot_rifle_running_state : public bc_xbot_state<bc_xbot_update_event, bc_xbot_weapon_detach_event>
	{
	public:
		explicit bc_xbot_rifle_running_state(game::bci_animation_job& p_animation_job)
			: m_animation_job(&p_animation_job)
		{
		}

		game::bci_animation_job& get_animation() noexcept override
		{
			return *m_animation_job;
		}

		void set_weapon(bc_xbot_weapon& p_weapon, const bcCHAR* p_offset_joint, const core::bc_vector3f& p_offset)
		{
			auto* l_rifle_running_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(static_cast<game::bc_sequence_animation_job&>(*m_animation_job));

			l_rifle_running_ik_job->set_offset_joint(p_offset_joint);
			l_rifle_running_ik_job->set_offset(p_offset);
		}

	private:
		state_transition handle(bc_xbot_update_event& p_event) override
		{
			update_directions(p_event);

			if (p_event.m_move_amount <= 0)
			{
				return state_transition::transfer_to<bc_xbot_rifle_idle_state>();
			}

			return state_transition::empty();
		}

		state_transition handle(bc_xbot_weapon_detach_event& p_event) override
		{
			return state_transition::transfer_to<bc_xbot_running_state>();
		}
		
		game::bci_animation_job* m_animation_job;
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
			m_look_speed(p_look_speed),
			m_run_speed(p_run_speed),
			m_walk_speed(p_walk_speed),
			m_move_speed(0),
			m_look_direction(p_local_forward),
			m_move_direction(p_local_forward),
			m_move_amount(0)
		{
		}

		bc_xbot_state_machine(bc_xbot_state_machine&&) noexcept = default;

		~bc_xbot_state_machine() = default;

		bc_xbot_state_machine& operator=(bc_xbot_state_machine&&) noexcept = default;

		void update(const bc_xbot_state_update& p_update)
		{
			bc_xbot_update_event l_update
			{
				p_update.m_clock,
				m_look_speed,
				m_run_speed,
				m_walk_speed,
				p_update.m_look_delta_x,
				p_update.m_look_velocity,
				p_update.m_forward_velocity,
				p_update.m_right_velocity,
				p_update.m_left_velocity,
				p_update.m_backward_velocity,
				p_update.m_walk_velocity,
				m_move_speed,
				m_look_direction,
				m_move_direction,
				m_move_amount
			};
			process_event(l_update);
		}

		bcFLOAT m_look_speed;
		bcFLOAT m_run_speed;
		bcFLOAT m_walk_speed;
		bcFLOAT m_move_speed;
		core::bc_vector3f m_look_direction;
		core::bc_vector3f m_move_direction;
		bcFLOAT m_move_amount;
	};
}
