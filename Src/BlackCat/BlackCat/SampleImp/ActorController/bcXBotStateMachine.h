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
		game::bci_animation_job* m_active_animation;
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

	class bc_xbot_idle_state : public core::bc_state<bc_xbot_state_machine, bc_xbot_update_event, bc_xbot_weapon_attach_event>
	{
	public:
		explicit bc_xbot_idle_state(game::bci_animation_job& p_animation);

	private:
		state_transition handle(bc_xbot_update_event& p_event) override;

		state_transition handle(bc_xbot_weapon_attach_event& p_event) override;

		void on_enter() override;
		
		game::bci_animation_job* m_animation;
	};

	class bc_xbot_running_state : public core::bc_state<bc_xbot_state_machine, bc_xbot_update_event, bc_xbot_weapon_attach_event>
	{
	public:
		explicit bc_xbot_running_state(game::bci_animation_job& p_animation);

	private:
		state_transition handle(bc_xbot_update_event& p_event) override;

		state_transition handle(bc_xbot_weapon_attach_event& p_event) override;

		void on_enter() override;
		
		game::bci_animation_job* m_animation;
	};

	class bc_xbot_rifle_idle_state : public core::bc_state<bc_xbot_state_machine, bc_xbot_update_event, bc_xbot_weapon_shoot_event, bc_xbot_weapon_detach_event>
	{
	public:
		explicit bc_xbot_rifle_idle_state(game::bci_animation_job& p_animation);

		void set_weapon(bc_xbot_weapon& p_weapon, const bcCHAR* p_offset_joint, const core::bc_vector3f& p_offset);

	private:
		state_transition handle(bc_xbot_update_event& p_event) override;

		state_transition handle(bc_xbot_weapon_shoot_event& p_event) override;

		state_transition handle(bc_xbot_weapon_detach_event& p_event) override;

		void on_enter() override;
		
		game::bci_animation_job* m_animation;
		bc_xbot_weapon* m_weapon;
		const bcCHAR* m_offset_joint;
		core::bc_vector3f m_offset;
	};

	class bc_xbot_rifle_running_state : public core::bc_state<bc_xbot_state_machine, bc_xbot_update_event, bc_xbot_weapon_shoot_event, bc_xbot_weapon_detach_event>
	{
	public:
		explicit bc_xbot_rifle_running_state(game::bci_animation_job& p_animation);

		void set_weapon(bc_xbot_weapon& p_weapon, const bcCHAR* p_offset_joint, const core::bc_vector3f& p_offset);

	private:
		state_transition handle(bc_xbot_update_event& p_event) override;

		state_transition handle(bc_xbot_weapon_shoot_event& p_event) override;

		state_transition handle(bc_xbot_weapon_detach_event& p_event) override;

		void on_enter() override;
		
		game::bci_animation_job* m_animation;
		bc_xbot_weapon* m_weapon;
		const bcCHAR* m_offset_joint;
		core::bc_vector3f m_offset;
	};

	class bc_xbot_state_machine : public core::bc_state_machine
	<
		bc_xbot_idle_state,
		bc_xbot_running_state,
		bc_xbot_rifle_idle_state,
		bc_xbot_rifle_running_state
	>
	{
	public:
		friend class bc_xbot_idle_state;
		friend class bc_xbot_running_state;
		friend class bc_xbot_rifle_idle_state;
		friend class bc_xbot_rifle_running_state;
	
	public:
		bc_xbot_state_machine(const core::bc_vector3f& p_local_forward,
			bcFLOAT p_look_speed,
			bcFLOAT p_run_speed,
			bcFLOAT p_walk_speed,
			game::bci_animation_job& p_idle_animation,
			game::bci_animation_job& p_running_animation,
			game::bci_animation_job& p_rifle_idle_animation,
			game::bci_animation_job& p_rifle_running_animation);

		bc_xbot_state_machine(bc_xbot_state_machine&&) noexcept;

		~bc_xbot_state_machine();

		bc_xbot_state_machine& operator=(bc_xbot_state_machine&&) noexcept;

		void update(const bc_xbot_state_update_params& p_update);

		void attach_weapon(bc_xbot_weapon& p_weapon, const bcCHAR* p_offset_joint, const core::bc_vector3f& p_offset);

		void detach_weapon();

		void shoot_weapon();

		game::bci_animation_job& get_active_animation() const noexcept;

		bc_xbot_state m_state;
	
	private:
		void update_directions(const bc_xbot_state_update_params& p_update_params) noexcept;

		void blend_idle_animation(const bc_xbot_state_update_params& p_update_params, game::bci_animation_job& p_idle_animation) noexcept;

		void blend_running_animation(const bc_xbot_state_update_params& p_update_params, game::bci_animation_job& p_running_animation) noexcept;

		void place_weapon_ik(bc_xbot_weapon& p_weapon, game::bci_animation_job& p_active_animation) noexcept;

		void blend_weapon_shoot_animation(const bc_xbot_state_update_params& p_update_params, bc_xbot_weapon& p_weapon, game::bci_animation_job& p_active_animation) noexcept;

		void blend_weapon_shoot(game::bci_animation_job& p_active_animation) noexcept;

		void change_animation(game::bci_animation_job* p_new_animation) noexcept;

		game::bci_animation_job* m_new_active_animation;
	};

	inline bc_xbot_idle_state::bc_xbot_idle_state(game::bci_animation_job& p_animation)
		: m_animation(&p_animation)
	{
	}

	inline bc_xbot_idle_state::state_transition bc_xbot_idle_state::handle(bc_xbot_update_event& p_event)
	{
		auto& l_machine = get_machine();

		l_machine.update_directions(p_event.m_update_params);
		l_machine.blend_idle_animation(p_event.m_update_params, *m_animation);
		
		if (l_machine.m_state.m_move_amount > 0)
		{
			return state_transition::transfer_to<bc_xbot_running_state>();
		}

		return state_transition::empty();
	}

	inline bc_xbot_idle_state::state_transition bc_xbot_idle_state::handle(bc_xbot_weapon_attach_event& p_event)
	{
		return state_transition::transfer_to<bc_xbot_rifle_idle_state>
		(
			[&](bc_xbot_rifle_idle_state& p_state)
			{
				p_state.set_weapon(p_event.m_weapon, p_event.m_offset_joint, p_event.m_offset);
			}
		);
	}

	inline void bc_xbot_idle_state::on_enter()
	{
		get_machine().change_animation(m_animation);
	}

	inline bc_xbot_running_state::bc_xbot_running_state(game::bci_animation_job& p_animation)
		: m_animation(&p_animation)
	{
	}

	inline bc_xbot_running_state::state_transition bc_xbot_running_state::handle(bc_xbot_update_event& p_event)
	{
		auto& l_machine = get_machine();
		
		l_machine.update_directions(p_event.m_update_params);
		l_machine.blend_running_animation(p_event.m_update_params, *m_animation);
		
		if (l_machine.m_state.m_move_amount <= 0)
		{
			return state_transition::transfer_to<bc_xbot_idle_state>();
		}

		return state_transition::empty();
	}

	inline bc_xbot_running_state::state_transition bc_xbot_running_state::handle(bc_xbot_weapon_attach_event& p_event)
	{
		return state_transition::transfer_to<bc_xbot_rifle_running_state>
		(
			[&](bc_xbot_rifle_running_state& p_state)
			{
				p_state.set_weapon(p_event.m_weapon, p_event.m_offset_joint, p_event.m_offset);
			}
		);
	}

	inline void bc_xbot_running_state::on_enter()
	{
		get_machine().change_animation(m_animation);
	}

	inline bc_xbot_rifle_idle_state::bc_xbot_rifle_idle_state(game::bci_animation_job& p_animation)
		: m_animation(&p_animation),
		m_weapon(nullptr),
		m_offset_joint(nullptr)
	{
	}

	inline void bc_xbot_rifle_idle_state::set_weapon(bc_xbot_weapon& p_weapon, const bcCHAR* p_offset_joint, const core::bc_vector3f& p_offset)
	{
		m_weapon = &p_weapon;
		m_offset_joint = p_offset_joint;
		m_offset = p_offset;
		
		auto* l_rifle_idle_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(static_cast<game::bc_sequence_animation_job&>(*m_animation));
		l_rifle_idle_ik_job->set_offset_joint(m_offset_joint);
		l_rifle_idle_ik_job->set_offset(m_offset);
	}

	inline bc_xbot_rifle_idle_state::state_transition bc_xbot_rifle_idle_state::handle(bc_xbot_update_event& p_event)
	{
		auto& l_machine = get_machine();
		
		l_machine.update_directions(p_event.m_update_params);
		l_machine.blend_idle_animation(p_event.m_update_params, *m_animation);
		l_machine.place_weapon_ik(*m_weapon, *m_animation);
		l_machine.blend_weapon_shoot_animation(p_event.m_update_params, *m_weapon, *m_animation);
		
		if (l_machine.m_state.m_move_amount > 0)
		{
			return state_transition::transfer_to<bc_xbot_rifle_running_state>
			(
				[&](bc_xbot_rifle_running_state& p_state)
				{
					p_state.set_weapon(*m_weapon, m_offset_joint, m_offset);
				}
			);
		}

		return state_transition::empty();
	}

	inline bc_xbot_rifle_idle_state::state_transition bc_xbot_rifle_idle_state::handle(bc_xbot_weapon_shoot_event& p_event)
	{
		get_machine().blend_weapon_shoot(*m_animation);
		return state_transition::empty();
	}

	inline bc_xbot_rifle_idle_state::state_transition bc_xbot_rifle_idle_state::handle(bc_xbot_weapon_detach_event& p_event)
	{
		return state_transition::transfer_to<bc_xbot_idle_state>();
	}

	inline void bc_xbot_rifle_idle_state::on_enter()
	{
		get_machine().change_animation(m_animation);
	}

	inline bc_xbot_rifle_running_state::bc_xbot_rifle_running_state(game::bci_animation_job& p_animation)
		: m_animation(&p_animation),
		m_weapon(nullptr),
		m_offset_joint(nullptr)
	{
	}

	inline void bc_xbot_rifle_running_state::set_weapon(bc_xbot_weapon& p_weapon, const bcCHAR* p_offset_joint, const core::bc_vector3f& p_offset)
	{
		m_weapon = &p_weapon;
		m_offset_joint = p_offset_joint;
		m_offset = p_offset;
		
		auto* l_rifle_running_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(static_cast<game::bc_sequence_animation_job&>(*m_animation));
		l_rifle_running_ik_job->set_offset_joint(p_offset_joint);
		l_rifle_running_ik_job->set_offset(p_offset);
	}

	inline bc_xbot_rifle_running_state::state_transition bc_xbot_rifle_running_state::handle(bc_xbot_update_event& p_event)
	{
		auto& l_machine = get_machine();
		
		l_machine.update_directions(p_event.m_update_params);
		l_machine.blend_running_animation(p_event.m_update_params, *m_animation);
		l_machine.place_weapon_ik(*m_weapon, *m_animation);
		l_machine.blend_weapon_shoot_animation(p_event.m_update_params, *m_weapon, *m_animation);
		
		if (l_machine.m_state.m_move_amount <= 0)
		{
			return state_transition::transfer_to<bc_xbot_rifle_idle_state>
			(
				[&](bc_xbot_rifle_idle_state& p_state)
				{
					p_state.set_weapon(*m_weapon, m_offset_joint, m_offset);
				}
			);
		}

		return state_transition::empty();
	}

	inline bc_xbot_rifle_running_state::state_transition bc_xbot_rifle_running_state::handle(bc_xbot_weapon_shoot_event& p_event)
	{
		get_machine().blend_weapon_shoot(*m_animation);
		return state_transition::empty();
	}

	inline bc_xbot_rifle_running_state::state_transition bc_xbot_rifle_running_state::handle(bc_xbot_weapon_detach_event& p_event)
	{
		return state_transition::transfer_to<bc_xbot_running_state>();
	}

	inline void bc_xbot_rifle_running_state::on_enter()
	{
		get_machine().change_animation(m_animation);
	}

	inline bc_xbot_state_machine::bc_xbot_state_machine(const core::bc_vector3f& p_local_forward,
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
			bc_xbot_running_state(p_running_animation),
			bc_xbot_rifle_idle_state(p_rifle_idle_animation),
			bc_xbot_rifle_running_state(p_rifle_running_animation)
		),
		m_state{p_look_speed, p_run_speed, p_walk_speed, p_local_forward, p_local_forward, 0, 0, -1, nullptr},
		m_new_active_animation(nullptr)
	{
		transfer_state<bc_xbot_idle_state>();
	}

	inline bc_xbot_state_machine::bc_xbot_state_machine(bc_xbot_state_machine&&) noexcept = default;
	
	inline bc_xbot_state_machine::~bc_xbot_state_machine() = default;
	
	inline bc_xbot_state_machine& bc_xbot_state_machine::operator=(bc_xbot_state_machine&&) noexcept = default;

	inline void bc_xbot_state_machine::update(const bc_xbot_state_update_params& p_update)
	{
		if(m_new_active_animation)
		{
			m_state.m_active_animation = m_new_active_animation;
			m_new_active_animation = nullptr;
		}
		
		bc_xbot_update_event l_event{ p_update };
		process_event(l_event);
	}

	inline void bc_xbot_state_machine::attach_weapon(bc_xbot_weapon& p_weapon, const bcCHAR* p_offset_joint, const core::bc_vector3f& p_offset)
	{
		bc_xbot_weapon_attach_event l_event{ p_weapon, p_offset_joint, p_offset };
		process_event(l_event);
	}

	inline void bc_xbot_state_machine::detach_weapon()
	{
		bc_xbot_weapon_detach_event l_event{};
		process_event(l_event);
	}

	inline void bc_xbot_state_machine::shoot_weapon()
	{
		bc_xbot_weapon_shoot_event l_event{ m_state };
		process_event(l_event);
	}

	inline game::bci_animation_job& bc_xbot_state_machine::get_active_animation() const noexcept
	{
		return *m_state.m_active_animation;
	}

	inline void bc_xbot_state_machine::update_directions(const bc_xbot_state_update_params& p_update_params) noexcept
	{
		core::bc_matrix3f l_look_rotation;
		core::bc_vector3f l_right_vector;
		core::bc_vector3f l_left_vector;

		if constexpr (graphic::bc_render_api_info::use_left_handed())
		{
			l_look_rotation.rotation_y_lh
			(
				core::bc_to_radian
				(
					p_update_params.m_look_delta_x *
					p_update_params.m_look_velocity *
					m_state.m_look_speed *
					p_update_params.m_clock.m_elapsed_second
				)
			);
		}
		else
		{
			l_look_rotation.rotation_y_rh
			(
				core::bc_to_radian
				(
					p_update_params.m_look_delta_x *
					p_update_params.m_look_velocity *
					m_state.m_look_speed *
					p_update_params.m_clock.m_elapsed_second
				)
			);
		}

		m_state.m_look_direction = core::bc_vector3f::normalize(l_look_rotation * m_state.m_look_direction);

		if constexpr (graphic::bc_render_api_info::use_left_handed())
		{
			core::bc_matrix3f l_right_left_direction;

			l_right_left_direction.rotation_y_lh(core::bc_to_radian(p_update_params.m_right_velocity * 90));
			l_right_vector = core::bc_vector3f::normalize(l_right_left_direction * m_state.m_look_direction);

			l_right_left_direction.rotation_y_lh(core::bc_to_radian(p_update_params.m_left_velocity * -90));
			l_left_vector = core::bc_vector3f::normalize(l_right_left_direction * m_state.m_look_direction);
		}
		else
		{
			core::bc_matrix3f l_right_left_direction;

			l_right_left_direction.rotation_y_rh(core::bc_to_radian(p_update_params.m_right_velocity * 90));
			l_right_vector = core::bc_vector3f::normalize(l_right_left_direction * m_state.m_look_direction);

			l_right_left_direction.rotation_y_rh(core::bc_to_radian(p_update_params.m_left_velocity * -90));
			l_left_vector = core::bc_vector3f::normalize(l_right_left_direction * m_state.m_look_direction);
		}

		m_state.m_move_speed = (1 - p_update_params.m_walk_velocity) * m_state.m_run_speed + p_update_params.m_walk_velocity * m_state.m_walk_speed;
		m_state.m_move_direction = m_state.m_look_direction * p_update_params.m_forward_velocity +
				m_state.m_look_direction * -p_update_params.m_backward_velocity +
				l_right_vector * p_update_params.m_right_velocity +
				l_left_vector * p_update_params.m_left_velocity;
		m_state.m_move_amount = std::max
		(
			{
				p_update_params.m_forward_velocity,
				p_update_params.m_backward_velocity,
				p_update_params.m_right_velocity,
				p_update_params.m_left_velocity
			}
		) * m_state.m_move_speed;

		if (core::bc_vector3f::length_sq(m_state.m_move_direction) <= 0)
		{
			m_state.m_move_amount = 0;
		}

		if (m_state.m_move_amount <= 0)
		{
			m_state.m_move_direction = m_state.m_look_direction;
		}

		m_state.m_move_direction.normalize();

		// if two vectors are exactly opposite of each other 'rotation_between_two_vector' method in matrix3x3 will have undefined result
		if (m_state.m_move_direction.dot(m_state.m_look_direction) >= -1.f)
		{
			m_state.m_move_direction.x -= 0.01f;
			m_state.m_move_direction.normalize();
		}
	}

	inline void bc_xbot_state_machine::blend_idle_animation(const bc_xbot_state_update_params& p_update_params, game::bci_animation_job& p_idle_animation) noexcept
	{
		const auto l_look_velocity = p_update_params.m_look_velocity;
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
		l_aim_job->set_world_target(p_update_params.m_position + m_state.m_look_direction * 1000);
	}

	inline void bc_xbot_state_machine::blend_running_animation(const bc_xbot_state_update_params& p_update_params, game::bci_animation_job& p_running_animation) noexcept
	{
		const auto l_run_weight = (m_state.m_move_speed - m_state.m_walk_speed) / (m_state.m_run_speed - m_state.m_walk_speed);
		const auto l_walk_weight = 1 - l_run_weight;
		const auto l_move_speed_normalize = m_state.m_move_amount / m_state.m_move_speed;
		bcFLOAT l_weights[] =
		{
			1 - l_move_speed_normalize,
			l_move_speed_normalize * l_walk_weight,
			l_move_speed_normalize * l_walk_weight,
			l_move_speed_normalize * l_run_weight,
			l_move_speed_normalize * l_run_weight,
		};

		const auto l_move_direction_dot = m_state.m_look_direction.dot(m_state.m_move_direction);
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
		l_aim_job->set_world_target(p_update_params.m_position + m_state.m_look_direction * 1000);
	}

	inline void bc_xbot_state_machine::place_weapon_ik(bc_xbot_weapon& p_weapon, game::bci_animation_job& p_active_animation) noexcept
	{
		auto* l_weapon_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(static_cast<game::bc_sequence_animation_job&>(p_active_animation));
		if (!l_weapon_ik_job)
		{
			return;
		}

		l_weapon_ik_job->set_weapon(&p_weapon);
	}

	inline void bc_xbot_state_machine::blend_weapon_shoot_animation(const bc_xbot_state_update_params& p_update_params, bc_xbot_weapon& p_weapon, game::bci_animation_job& p_active_animation) noexcept
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

		if (m_state.m_weapon_shoot_time >= 0)
		{
			m_state.m_weapon_shoot_time += p_update_params.m_clock.m_elapsed_second;

			if (m_state.m_weapon_shoot_time >= p_weapon.m_rate_of_fire_seconds)
			{
				m_state.m_weapon_shoot_time = -1;
			}
		}

		// Increase weapon shoot impact in running mode
		l_additive_blend_job->set_weights(1, m_state.m_move_amount ? std::max((m_state.m_move_amount / m_state.m_run_speed) * 2, 1.4f) : 1);

		// Once weapon shoot animation stopped disable additive blend
		if (!l_additive_layer->get_enabled())
		{
			l_additive_layer->set_local_time(0);
			l_additive_blend_job->set_enabled(false);
		}
	}

	inline void bc_xbot_state_machine::blend_weapon_shoot(game::bci_animation_job& p_active_animation) noexcept
	{
		auto* l_additive_blend_job = game::bc_animation_job_helper::find_job<game::bc_additive_blending_animation_job>(static_cast<game::bc_sequence_animation_job&>(p_active_animation));
		if (!l_additive_blend_job || !l_additive_blend_job->get_additive_layer())
		{
			return;
		}

		if (m_state.m_weapon_shoot_time >= 0)
		{
			return;
		}

		m_state.m_weapon_shoot_time = 0;
		l_additive_blend_job->get_additive_layer()->set_local_time(0);
		l_additive_blend_job->get_additive_layer()->set_enabled(true);
		l_additive_blend_job->set_enabled(true);
	}

	inline void bc_xbot_state_machine::change_animation(game::bci_animation_job* p_new_animation) noexcept
	{
		m_new_active_animation = p_new_animation;
	}
}