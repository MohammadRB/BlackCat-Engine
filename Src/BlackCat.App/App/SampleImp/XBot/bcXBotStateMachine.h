// [10/09/2021 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/Math/bcCoordinate.h"
#include "Core/Container/bcStateMachine.h"
#include "Core/Utility/bcVelocity.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/System/Animation/bcAnimationJob.h"
#include "Game/System/Animation/bcAnimationJobHelper.h"
#include "Game/System/Animation/Job/bcBlendingAnimationJob.h"
#include "Game/System/Animation/Job/bcPartialBlendingAnimationJob.h"
#include "Game/System/Animation/Job/bcAdditiveBlendingAnimationJob.h"
#include "Game/System/Animation/Job/bcExecuteOneAnimationJob.h"
#include "App/SampleImp/XBot/bcXBotWeaponIKAnimationJob.h"

namespace black_cat
{
	class bc_xbot_state_machine;
	class bc_xbot_idle_state;
	class bc_xbot_running_state;
	class bc_xbot_rifle_idle_state;
	class bc_xbot_rifle_running_state;
	class bc_xbot_ragdoll_state;

	struct bc_xbot_state
	{
		bcINT32 m_look_side{0};
		core::bc_vector3f m_look_direction;
		core::bc_vector3f m_move_direction;
		bcFLOAT m_move_speed{0};
		bcFLOAT m_move_amount{0};
		bcFLOAT m_grenade_throw_time{-1};
		bcFLOAT m_weapon_shoot_time{-1};
		core::bc_velocity<bcFLOAT> m_grenade_throw_weight{0,1,1};
		game::bci_animation_job* m_active_animation{nullptr};
	};

	struct bc_xbot_state_update_params
	{
		const platform::bc_clock::update_param& m_clock;
		const core::bc_vector3f& m_position;
		bcINT32 m_look_delta_x;
		bcFLOAT m_look_velocity;
		bcFLOAT m_forward_velocity;
		bcFLOAT m_backward_velocity;
		bcFLOAT m_right_velocity;
		bcFLOAT m_left_velocity;
		bcFLOAT m_walk_velocity;
	};

	struct bc_xbot_state_update_params1
	{
		const platform::bc_clock::update_param& m_clock;
		const core::bc_vector3f& m_position;
		const core::bc_vector3f& m_look_direction;
		bcINT32 m_look_side;
		bcFLOAT m_look_velocity;
		bcFLOAT m_forward_velocity;
		bcFLOAT m_backward_velocity;
		bcFLOAT m_right_velocity;
		bcFLOAT m_left_velocity;
		bcFLOAT m_walk_velocity;
	};
	
	struct bc_xbot_update_event
	{
		const bc_xbot_state_update_params& m_update_params;
	};

	struct bc_xbot_grenade_throw_event
	{
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

	class bc_xbot_idle_state : public core::bc_state<bc_xbot_state_machine, bc_xbot_update_event, bc_xbot_grenade_throw_event, bc_xbot_weapon_attach_event>
	{
	public:
		explicit bc_xbot_idle_state(game::bci_animation_job& p_animation);

	private:
		state_transition handle(bc_xbot_update_event& p_event) override;

		state_transition handle(bc_xbot_grenade_throw_event& p_event) override;
		
		state_transition handle(bc_xbot_weapon_attach_event& p_event) override;

		void on_enter() override;

		game::bci_animation_job* m_animation;
	};

	class bc_xbot_running_state : public core::bc_state<bc_xbot_state_machine, bc_xbot_update_event, bc_xbot_grenade_throw_event, bc_xbot_weapon_attach_event>
	{
	public:
		explicit bc_xbot_running_state(game::bci_animation_job& p_animation);

	private:
		state_transition handle(bc_xbot_update_event& p_event) override;

		state_transition handle(bc_xbot_grenade_throw_event& p_event) override;
		
		state_transition handle(bc_xbot_weapon_attach_event& p_event) override;

		void on_enter() override;

		game::bci_animation_job* m_animation;
	};

	class bc_xbot_rifle_idle_state : public core::bc_state<bc_xbot_state_machine, bc_xbot_update_event, bc_xbot_grenade_throw_event, bc_xbot_weapon_shoot_event, bc_xbot_weapon_detach_event>
	{
	public:
		explicit bc_xbot_rifle_idle_state(game::bci_animation_job& p_animation);

		void set_weapon(bc_xbot_weapon& p_weapon, const bcCHAR* p_offset_joint, const core::bc_vector3f& p_offset);

	private:
		state_transition handle(bc_xbot_update_event& p_event) override;

		state_transition handle(bc_xbot_grenade_throw_event& p_event) override;

		state_transition handle(bc_xbot_weapon_shoot_event& p_event) override;

		state_transition handle(bc_xbot_weapon_detach_event& p_event) override;

		void on_enter() override;

		game::bci_animation_job* m_animation;
		bc_xbot_weapon* m_weapon;
		const bcCHAR* m_offset_joint;
		core::bc_vector3f m_offset;
	};

	class bc_xbot_rifle_running_state : public core::bc_state<bc_xbot_state_machine, bc_xbot_update_event, bc_xbot_grenade_throw_event, bc_xbot_weapon_shoot_event, bc_xbot_weapon_detach_event>
	{
	public:
		explicit bc_xbot_rifle_running_state(game::bci_animation_job& p_animation);

		void set_weapon(bc_xbot_weapon& p_weapon, const bcCHAR* p_offset_joint, const core::bc_vector3f& p_offset);

	private:
		state_transition handle(bc_xbot_update_event& p_event) override;

		state_transition handle(bc_xbot_grenade_throw_event& p_event) override;
		
		state_transition handle(bc_xbot_weapon_shoot_event& p_event) override;

		state_transition handle(bc_xbot_weapon_detach_event& p_event) override;

		void on_enter() override;

		game::bci_animation_job* m_animation;
		bc_xbot_weapon* m_weapon;
		const bcCHAR* m_offset_joint;
		core::bc_vector3f m_offset;
	};

	class bc_xbot_ragdoll_state : public core::bc_state<bc_xbot_state_machine, bc_xbot_update_event>
	{
	private:
		state_transition handle(bc_xbot_update_event& p_event) override;

		void on_enter() override;
	};

	class bc_xbot_state_machine : public core::bc_state_machine
	<
		bc_xbot_idle_state,
		bc_xbot_running_state,
		bc_xbot_rifle_idle_state,
		bc_xbot_rifle_running_state,
		bc_xbot_ragdoll_state
	>
	{
	public:
		friend class bc_xbot_idle_state;
		friend class bc_xbot_running_state;
		friend class bc_xbot_rifle_idle_state;
		friend class bc_xbot_rifle_running_state;
		friend class bc_xbot_ragdoll_state;
	
	public:
		bc_xbot_state_machine(const core::bc_vector3f& p_local_forward,
			bcFLOAT p_look_speed,
			bcFLOAT p_run_speed,
			bcFLOAT p_walk_speed,
			bcFLOAT p_mass,
			game::bci_animation_job& p_animation_pipeline);

		bc_xbot_state_machine(bc_xbot_state_machine&&) noexcept;

		~bc_xbot_state_machine();

		bc_xbot_state_machine& operator=(bc_xbot_state_machine&&) noexcept;

		void update(const bc_xbot_state_update_params& p_update);
		
		void update(const bc_xbot_state_update_params1& p_update);

		bool throw_grenade() noexcept;
		
		void attach_weapon(bc_xbot_weapon& p_weapon, const bcCHAR* p_offset_joint, const core::bc_vector3f& p_offset) noexcept;

		void detach_weapon() noexcept;

		bool shoot_weapon() noexcept;

		game::bci_animation_job* get_active_animation() const noexcept;

		bc_xbot_state m_state;
	
	private:
		void update_directions(const bc_xbot_state_update_params& p_update_params, bcFLOAT p_holding_mass = 0) noexcept;

		void blend_without_weapon_animation(game::bci_animation_job& p_animation) noexcept;
		
		void blend_with_weapon_animation(game::bci_animation_job& p_animation) noexcept;
		
		void blend_idle_animation(const bc_xbot_state_update_params& p_update_params, game::bc_blending_animation_job& p_idle_blending) noexcept;

		void blend_running_animation(const bc_xbot_state_update_params& p_update_params, game::bc_blending_animation_job& p_running_blending) noexcept;

		void blend_aim_animation(const bc_xbot_state_update_params& p_update_params, game::bci_animation_job& p_animation);
		
		void start_blend_grenade_throw(game::bci_animation_job& p_active_animation);
		
		void blend_grenade_throw_animation(const bc_xbot_state_update_params& p_update_params, game::bci_animation_job& p_active_animation) noexcept;

		void start_blend_weapon_shoot(game::bci_animation_job& p_active_animation) noexcept;

		void blend_weapon_shoot_animation(const bc_xbot_state_update_params& p_update_params, bc_xbot_weapon& p_weapon, game::bci_animation_job& p_active_animation) noexcept;

		void place_weapon_ik(bc_xbot_weapon& p_weapon, game::bci_animation_job& p_active_animation) noexcept;

		void change_animation(game::bci_animation_job* p_new_animation) noexcept;

		bcFLOAT m_look_speed;
		bcFLOAT m_run_speed;
		bcFLOAT m_walk_speed;
		bcFLOAT m_mass;
		game::bci_animation_job* m_new_active_animation;
	};

	inline bc_xbot_idle_state::bc_xbot_idle_state(game::bci_animation_job& p_animation)
		: m_animation(&p_animation)
	{
	}

	inline bc_xbot_idle_state::state_transition bc_xbot_idle_state::handle(bc_xbot_update_event& p_event)
	{
		auto& l_machine = get_machine();
		auto* l_idle_blending_job = game::bc_animation_job_helper::find_job<game::bc_blending_animation_job>(*m_animation, "idle_blending");

		l_machine.update_directions(p_event.m_update_params);
		l_machine.blend_without_weapon_animation(*m_animation);
		l_machine.blend_idle_animation(p_event.m_update_params, *l_idle_blending_job);
		l_machine.blend_aim_animation(p_event.m_update_params, *m_animation);
		l_machine.blend_grenade_throw_animation(p_event.m_update_params, *m_animation);
		
		if (l_machine.m_state.m_move_amount > 0)
		{
			return state_transition::transfer_to<bc_xbot_running_state>();
		}

		return state_transition::empty();
	}

	inline bc_xbot_idle_state::state_transition bc_xbot_idle_state::handle(bc_xbot_grenade_throw_event& p_event)
	{
		get_machine().start_blend_grenade_throw(*m_animation);
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
		auto* l_execute_one_job = game::bc_animation_job_helper::find_job<game::bc_execute_one_animation_job>(*m_animation, "main_local");
		l_execute_one_job->set_active_index(0);

		auto* l_weapon_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(*m_animation);
		if (l_weapon_ik_job)
		{
			l_weapon_ik_job->set_enabled(false);
		}
		
		get_machine().change_animation(m_animation);
	}

	inline bc_xbot_running_state::bc_xbot_running_state(game::bci_animation_job& p_animation)
		: m_animation(&p_animation)
	{
	}

	inline bc_xbot_running_state::state_transition bc_xbot_running_state::handle(bc_xbot_update_event& p_event)
	{
		auto& l_machine = get_machine();
		auto* l_running_blending_job = game::bc_animation_job_helper::find_job<game::bc_blending_animation_job>(*m_animation, "running_blending");
		
		l_machine.update_directions(p_event.m_update_params);
		l_machine.blend_without_weapon_animation(*m_animation);
		l_machine.blend_running_animation(p_event.m_update_params, *l_running_blending_job);
		l_machine.blend_aim_animation(p_event.m_update_params, *m_animation);
		l_machine.blend_grenade_throw_animation(p_event.m_update_params, *m_animation);
		
		if (l_machine.m_state.m_move_amount <= 0)
		{
			return state_transition::transfer_to<bc_xbot_idle_state>();
		}

		return state_transition::empty();
	}

	inline bc_xbot_running_state::state_transition bc_xbot_running_state::handle(bc_xbot_grenade_throw_event& p_event)
	{
		get_machine().start_blend_grenade_throw(*m_animation);
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
		auto* l_execute_one_job = game::bc_animation_job_helper::find_job<game::bc_execute_one_animation_job>(*m_animation, "main_local");
		l_execute_one_job->set_active_index(0);

		auto* l_weapon_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(*m_animation);
		if (l_weapon_ik_job)
		{
			l_weapon_ik_job->set_enabled(false);
		}
		
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
		
		auto* l_rifle_idle_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(*m_animation);
		l_rifle_idle_ik_job->set_offset_joint(m_offset_joint, m_offset);
	}

	inline bc_xbot_rifle_idle_state::state_transition bc_xbot_rifle_idle_state::handle(bc_xbot_update_event& p_event)
	{
		auto& l_machine = get_machine();
		auto* l_idle_blending_job = game::bc_animation_job_helper::find_job<game::bc_blending_animation_job>(*m_animation, "weapon_idle_blending");

		l_machine.update_directions(p_event.m_update_params, m_weapon->m_mass);
		l_machine.blend_with_weapon_animation(*m_animation);
		l_machine.blend_idle_animation(p_event.m_update_params, *l_idle_blending_job);
		l_machine.blend_aim_animation(p_event.m_update_params, *m_animation);
		l_machine.blend_grenade_throw_animation(p_event.m_update_params, *m_animation);
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

	inline bc_xbot_rifle_idle_state::state_transition bc_xbot_rifle_idle_state::handle(bc_xbot_grenade_throw_event& p_event)
	{
		get_machine().start_blend_grenade_throw(*m_animation);
		return state_transition::empty();
	}

	inline bc_xbot_rifle_idle_state::state_transition bc_xbot_rifle_idle_state::handle(bc_xbot_weapon_shoot_event& p_event)
	{
		get_machine().start_blend_weapon_shoot(*m_animation);
		return state_transition::empty();
	}

	inline bc_xbot_rifle_idle_state::state_transition bc_xbot_rifle_idle_state::handle(bc_xbot_weapon_detach_event& p_event)
	{
		return state_transition::transfer_to<bc_xbot_idle_state>();
	}

	inline void bc_xbot_rifle_idle_state::on_enter()
	{
		auto* l_execute_one_job = game::bc_animation_job_helper::find_job<game::bc_execute_one_animation_job>(*m_animation, "main_local");
		l_execute_one_job->set_active_index(1);

		auto* l_weapon_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(*m_animation);
		if (l_weapon_ik_job)
		{
			l_weapon_ik_job->set_enabled(true);
		}
		
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
		
		auto* l_rifle_running_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(*m_animation);
		l_rifle_running_ik_job->set_offset_joint(p_offset_joint, m_offset);
	}

	inline bc_xbot_rifle_running_state::state_transition bc_xbot_rifle_running_state::handle(bc_xbot_update_event& p_event)
	{
		auto& l_machine = get_machine();
		auto* l_running_blending_job = game::bc_animation_job_helper::find_job<game::bc_blending_animation_job>(*m_animation, "weapon_running_blending");
		
		l_machine.update_directions(p_event.m_update_params, m_weapon->m_mass);
		l_machine.blend_with_weapon_animation(*m_animation);
		l_machine.blend_running_animation(p_event.m_update_params, *l_running_blending_job);
		l_machine.blend_aim_animation(p_event.m_update_params, *m_animation);
		l_machine.blend_grenade_throw_animation(p_event.m_update_params, *m_animation);
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

	inline bc_xbot_rifle_running_state::state_transition bc_xbot_rifle_running_state::handle(bc_xbot_grenade_throw_event& p_event)
	{
		get_machine().start_blend_grenade_throw(*m_animation);
		return state_transition::empty();
	}

	inline bc_xbot_rifle_running_state::state_transition bc_xbot_rifle_running_state::handle(bc_xbot_weapon_shoot_event& p_event)
	{
		get_machine().start_blend_weapon_shoot(*m_animation);
		return state_transition::empty();
	}

	inline bc_xbot_rifle_running_state::state_transition bc_xbot_rifle_running_state::handle(bc_xbot_weapon_detach_event& p_event)
	{
		return state_transition::transfer_to<bc_xbot_running_state>();
	}

	inline void bc_xbot_rifle_running_state::on_enter()
	{
		auto* l_execute_one_job = game::bc_animation_job_helper::find_job<game::bc_execute_one_animation_job>(*m_animation, "main_local");
		l_execute_one_job->set_active_index(1);

		auto* l_weapon_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(*m_animation);
		if (l_weapon_ik_job)
		{
			l_weapon_ik_job->set_enabled(true);
		}
		
		get_machine().change_animation(m_animation);
	}

	inline core::bc_state<bc_xbot_state_machine, bc_xbot_update_event>::state_transition bc_xbot_ragdoll_state::handle(bc_xbot_update_event& p_event)
	{
		auto& l_machine = get_machine();
		l_machine.update_directions(p_event.m_update_params);

		return state_transition::empty();
	}

	inline void bc_xbot_ragdoll_state::on_enter()
	{
		get_machine().m_state.m_active_animation = nullptr;
	}

	inline bc_xbot_state_machine::bc_xbot_state_machine(const core::bc_vector3f& p_local_forward,
		bcFLOAT p_look_speed,
		bcFLOAT p_run_speed,
		bcFLOAT p_walk_speed,
		bcFLOAT p_mass,
		game::bci_animation_job& p_animation_pipeline)
		: bc_state_machine
		(
			bc_xbot_idle_state(p_animation_pipeline),
			bc_xbot_running_state(p_animation_pipeline),
			bc_xbot_rifle_idle_state(p_animation_pipeline),
			bc_xbot_rifle_running_state(p_animation_pipeline),
			bc_xbot_ragdoll_state()
		),
		m_look_speed(p_look_speed),
		m_run_speed(p_run_speed),
		m_walk_speed(p_walk_speed),
		m_mass(p_mass),
		m_new_active_animation(nullptr)
	{
		m_state.m_look_side = 0;
		m_state.m_look_direction = p_local_forward;
		m_state.m_move_direction = p_local_forward;
		m_state.m_move_speed = 0;
		m_state.m_move_amount = 0;
		m_state.m_grenade_throw_time = -1;
		m_state.m_weapon_shoot_time = -1;
		m_state.m_grenade_throw_weight = core::bc_velocity<bcFLOAT>(0, 1, 0.3f);
		m_state.m_active_animation = nullptr;
		
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

		if(p_update.m_look_delta_x > 0)
		{
			m_state.m_look_side = 1;
		}
		else if(p_update.m_look_delta_x < 0)
		{
			m_state.m_look_side = -1;
		}
		
		bc_xbot_update_event l_event{ p_update };
		process_event(l_event);
	}

	inline void bc_xbot_state_machine::update(const bc_xbot_state_update_params1& p_update)
	{
		m_state.m_look_direction = p_update.m_look_direction;
		m_state.m_look_side = p_update.m_look_side;
		update(bc_xbot_state_update_params
		{
			p_update.m_clock,
			p_update.m_position,
			0,
			p_update.m_look_velocity,
			p_update.m_forward_velocity,
			p_update.m_backward_velocity,
			p_update.m_right_velocity,
			p_update.m_left_velocity,
			p_update.m_walk_velocity
		});
	}

	inline bool bc_xbot_state_machine::throw_grenade() noexcept
	{
		if (m_state.m_weapon_shoot_time >= 0 || m_state.m_grenade_throw_time >= 0)
		{
			return false;
		}
		
		bc_xbot_grenade_throw_event l_event{};
		process_event(l_event);

		return true;
	}

	inline void bc_xbot_state_machine::attach_weapon(bc_xbot_weapon& p_weapon, const bcCHAR* p_offset_joint, const core::bc_vector3f& p_offset) noexcept
	{
		bc_xbot_weapon_attach_event l_event{ p_weapon, p_offset_joint, p_offset };
		process_event(l_event);
	}

	inline void bc_xbot_state_machine::detach_weapon() noexcept
	{
		bc_xbot_weapon_detach_event l_event{};
		process_event(l_event);
	}

	inline bool bc_xbot_state_machine::shoot_weapon() noexcept
	{
		if(m_state.m_weapon_shoot_time >= 0 || m_state.m_grenade_throw_time >= 0)
		{
			return false;
		}
		
		bc_xbot_weapon_shoot_event l_event{ m_state };
		process_event(l_event);

		return true;
	}

	inline game::bci_animation_job* bc_xbot_state_machine::get_active_animation() const noexcept
	{
		return m_state.m_active_animation;
	}

	inline void bc_xbot_state_machine::update_directions(const bc_xbot_state_update_params& p_update_params, bcFLOAT p_holding_mass) noexcept
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
					static_cast<bcFLOAT>(p_update_params.m_look_delta_x) * p_update_params.m_look_velocity * m_look_speed * p_update_params.m_clock.m_elapsed_second
				)
			);
		}
		else
		{
			l_look_rotation.rotation_y_rh
			(
				core::bc_to_radian
				(
					static_cast<bcFLOAT>(p_update_params.m_look_delta_x) * p_update_params.m_look_velocity * m_look_speed * p_update_params.m_clock.m_elapsed_second
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

		const auto l_holding_mass = std::max(.6f, 1 - p_holding_mass / m_mass);
		m_state.m_move_speed = (1 - p_update_params.m_walk_velocity) * m_run_speed + p_update_params.m_walk_velocity * m_walk_speed;
		m_state.m_move_direction = m_state.m_look_direction * p_update_params.m_forward_velocity +
				m_state.m_look_direction * -p_update_params.m_backward_velocity +
				l_right_vector * p_update_params.m_right_velocity +
				l_left_vector * p_update_params.m_left_velocity;
		m_state.m_move_amount = l_holding_mass * m_state.m_move_speed *
				std::max
				(
					{
						p_update_params.m_forward_velocity,
						p_update_params.m_backward_velocity,
						p_update_params.m_right_velocity,
						p_update_params.m_left_velocity
					}
				);

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

	inline void bc_xbot_state_machine::blend_without_weapon_animation(game::bci_animation_job& p_animation) noexcept
	{
		const auto l_move_speed_normalize = m_state.m_move_amount / m_state.m_move_speed;
		
		bcFLOAT l_blend_weights[] = { 1 - l_move_speed_normalize, l_move_speed_normalize };
		auto* l_without_weapon_blend_job = game::bc_animation_job_helper::find_job<game::bc_blending_animation_job>(p_animation, "without_weapon_blending");
		l_without_weapon_blend_job->set_weights(l_blend_weights);
	}

	inline void bc_xbot_state_machine::blend_with_weapon_animation(game::bci_animation_job& p_animation) noexcept
	{
		const auto l_move_speed_normalize = m_state.m_move_amount / m_state.m_move_speed;

		bcFLOAT l_blend_weights[] = { 1 - l_move_speed_normalize, l_move_speed_normalize };
		auto* l_without_weapon_blend_job = game::bc_animation_job_helper::find_job<game::bc_blending_animation_job>(p_animation, "with_weapon_blending");
		l_without_weapon_blend_job->set_weights(l_blend_weights);
	}

	inline void bc_xbot_state_machine::blend_idle_animation(const bc_xbot_state_update_params& p_update_params, game::bc_blending_animation_job& p_idle_blending) noexcept
	{
		const auto l_look_velocity = p_update_params.m_look_velocity;
		bcFLOAT l_weights[3] = 
		{
			m_state.m_look_side < 0 ? l_look_velocity : 0.f,
			1 - l_look_velocity,
			m_state.m_look_side > 0 ? l_look_velocity : 0.f
		};

		p_idle_blending.set_weights(&l_weights[0]);

		if (l_look_velocity == 0.f)
		{
			bcFLOAT l_times[3] = { 0, -1, 0 };
			p_idle_blending.set_local_times(&l_times[0]);
		}
		else
		{
			bcFLOAT l_times[3] = { -1, 0, -1 };
			p_idle_blending.set_local_times(&l_times[0]);
		}
	}

	inline void bc_xbot_state_machine::blend_running_animation(const bc_xbot_state_update_params& p_update_params, game::bc_blending_animation_job& p_running_blending) noexcept
	{
		const auto l_run_weight = (m_state.m_move_speed - m_walk_speed) / (m_run_speed - m_walk_speed);
		const auto l_walk_weight = 1 - l_run_weight;
		bcFLOAT l_weights[] =
		{
			l_walk_weight,
			l_walk_weight,
			l_run_weight,
			l_run_weight,
		};

		const auto l_move_direction_dot = m_state.m_look_direction.dot(m_state.m_move_direction);
		if (l_move_direction_dot >= -0.01f)
		{
			l_weights[1] = 0;
			l_weights[3] = 0;
		}
		else
		{
			l_weights[0] = 0;
			l_weights[2] = 0;
		}

		p_running_blending.set_weights(&l_weights[0]);
	}

	inline void bc_xbot_state_machine::blend_aim_animation(const bc_xbot_state_update_params& p_update_params, game::bci_animation_job& p_animation)
	{
		auto* l_aim_job = game::bc_animation_job_helper::find_job<game::bc_aim_animation_job>(p_animation);
		l_aim_job->set_world_target(p_update_params.m_position + m_state.m_look_direction * 1000);
	}

	inline void bc_xbot_state_machine::start_blend_grenade_throw(game::bci_animation_job& p_active_animation)
	{
		if (m_state.m_grenade_throw_time >= 0)
		{
			return;
		}
		
		auto* l_grenade_throw_blend_job = game::bc_animation_job_helper::find_job<game::bc_blending_animation_job>(p_active_animation, "grenade_throw_blending");
		if(!l_grenade_throw_blend_job)
		{
			return;
		}

		bcFLOAT l_grenade_throw_blend_weights[] = { 1,0 };
		l_grenade_throw_blend_job->set_weights(&l_grenade_throw_blend_weights[0]);
		
		auto& l_grenade_throw_partial_blending = static_cast<game::bc_partial_blending_animation_job&>(l_grenade_throw_blend_job->get_layer(1));
		auto& l_grenade_throw_partial_layer = *l_grenade_throw_partial_blending.get_layer2();

		m_state.m_grenade_throw_time = 0;
		l_grenade_throw_partial_layer.set_local_time(0);
		l_grenade_throw_partial_layer.set_enabled(true);
		l_grenade_throw_partial_blending.set_enabled(true);
	}

	inline void bc_xbot_state_machine::blend_grenade_throw_animation(const bc_xbot_state_update_params& p_update_params, game::bci_animation_job& p_active_animation) noexcept
	{
		if (m_state.m_grenade_throw_time < 0)
		{
			return;
		}
		
		auto* l_grenade_throw_blend_job = game::bc_animation_job_helper::find_job<game::bc_blending_animation_job>(p_active_animation, "grenade_throw_blending");
		if(!l_grenade_throw_blend_job)
		{
			return;
		}
		
		auto& l_grenade_throw_partial_blending = static_cast<game::bc_partial_blending_animation_job&>(l_grenade_throw_blend_job->get_layer(1));
		auto& l_grenade_throw_partial_layer = *l_grenade_throw_partial_blending.get_layer2();
		
		m_state.m_grenade_throw_time += p_update_params.m_clock.m_elapsed_second;

		const auto l_is_not_ending = l_grenade_throw_partial_layer.get_animation()->get_duration() - m_state.m_grenade_throw_time > m_state.m_grenade_throw_weight.get_drag_time();
		if (l_is_not_ending)
		{
			m_state.m_grenade_throw_weight.push(p_update_params.m_clock.m_elapsed_second);
		}
		else
		{
			m_state.m_grenade_throw_weight.release(p_update_params.m_clock.m_elapsed_second);
		}

		// Give a minimum weight to let partial blending job to run so partial layer complete its cycle and disable itself
		const auto l_grenade_throw_weight = std::max(0.001f, m_state.m_grenade_throw_weight.get_value());
		bcFLOAT l_grenade_throw_blend_weights[] = { 1 - l_grenade_throw_weight, l_grenade_throw_weight };
		l_grenade_throw_blend_job->set_weights(&l_grenade_throw_blend_weights[0]);
		
		if(!l_grenade_throw_partial_layer.get_enabled())
		{
			m_state.m_grenade_throw_time = -1;
			l_grenade_throw_partial_layer.set_local_time(0);
			l_grenade_throw_partial_layer.set_enabled(false);
			l_grenade_throw_partial_blending.set_enabled(false);
		}
	}

	inline void bc_xbot_state_machine::start_blend_weapon_shoot(game::bci_animation_job& p_active_animation) noexcept
	{
		auto* l_additive_blend_job = game::bc_animation_job_helper::find_job<game::bc_additive_blending_animation_job>(p_active_animation);
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
	
	inline void bc_xbot_state_machine::blend_weapon_shoot_animation(const bc_xbot_state_update_params& p_update_params, bc_xbot_weapon& p_weapon, game::bci_animation_job& p_active_animation) noexcept
	{
		auto* l_additive_blend_job = game::bc_animation_job_helper::find_job<game::bc_additive_blending_animation_job>(p_active_animation);
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
		//const auto l_additive_layer_weight = m_state.m_move_amount > 0.f ? std::max((m_state.m_move_amount / m_run_speed) * 1.8f, 1.2f) : 1;
		l_additive_blend_job->set_weights(1, 1);

		// Once weapon shoot animation stopped disable additive blend
		if (!l_additive_layer->get_enabled())
		{
			l_additive_layer->set_local_time(0);
			l_additive_blend_job->set_enabled(false);
		}
	}

	inline void bc_xbot_state_machine::place_weapon_ik(bc_xbot_weapon& p_weapon, game::bci_animation_job& p_active_animation) noexcept
	{
		auto* l_weapon_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(p_active_animation);
		if (!l_weapon_ik_job)
		{
			return;
		}

		const auto l_grenade_throw_weight = m_state.m_grenade_throw_weight.get_value();
		l_weapon_ik_job->set_weight(1 - l_grenade_throw_weight);
		l_weapon_ik_job->set_weapon(&p_weapon);
	}

	inline void bc_xbot_state_machine::change_animation(game::bci_animation_job* p_new_animation) noexcept
	{
		m_new_active_animation = p_new_animation;
	}
}