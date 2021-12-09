// [10/16/2021 MRB]

#pragma once

#include "Game/Object/Scene/Component/bcHumanRagdollComponent.h"
#include "Game/Object/Scene/Component/bcRigidControllerComponent.h"
#include "BlackCat/SampleImp/XBot/bcXBotActorController.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_xbot_idle_actor_controller : public bc_xbot_actor_controller
	{
	public:
		void initialize(const game::bc_actor_component_initialize_context& p_context) override;
		
		void load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context) override;
		
		void load_replicated_network_instance(const game::bc_actor_component_network_load_context& p_context) override;
		
		void write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context) override;
		
		void write_replicated_network_instance(const game::bc_actor_component_network_write_context& p_context) override;
		
		void update_origin_instance(const game::bc_actor_component_update_content& p_context) override;
		
		void update_replicated_instance(const game::bc_actor_component_update_content& p_context) override;
		
	private:
		void throw_grenade(game::bc_actor& p_grenade) noexcept override;
		
		game::bc_human_ragdoll_component* m_ragdoll_component = nullptr;
		game::bc_rigid_controller_component* m_rigid_controller_component = nullptr;
		bool m_ragdoll_enabled = false;
		bcFLOAT m_ragdoll_timer = 0;
	};

	inline void bc_xbot_idle_actor_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		bc_xbot_actor_controller::initialize(p_context);
		
		m_ragdoll_component = p_context.m_actor.get_component<game::bc_human_ragdoll_component>();
		m_rigid_controller_component = p_context.m_actor.get_component<game::bc_rigid_controller_component>();
	}

	inline void bc_xbot_idle_actor_controller::load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
	}

	inline void bc_xbot_idle_actor_controller::load_replicated_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
	}

	inline void bc_xbot_idle_actor_controller::write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
	}

	inline void bc_xbot_idle_actor_controller::write_replicated_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
	}

	inline void bc_xbot_idle_actor_controller::update_origin_instance(const game::bc_actor_component_update_content& p_context)
	{
		if(m_ragdoll_component)
		{
			const auto l_prev_time = m_ragdoll_timer;
			m_ragdoll_timer += p_context.m_clock.m_elapsed_second;
			if (m_ragdoll_timer >= 2.f && l_prev_time < 2.f)
			{
				m_rigid_controller_component->reset_controller();
				m_ragdoll_component->set_enable(true);
				m_ragdoll_component->add_force(game::bc_human_ragdoll_component::s_head_index, core::bc_vector3f(0, .1f, 1) * 10000);
				m_ragdoll_enabled = true;
			}
		}

		if(!m_ragdoll_enabled)
		{
			bc_xbot_actor_controller::update(bc_xbot_input_update_context
			{
				p_context.m_clock,
				0,
				false,
				false,
				false,
				false,
				false
			});
		}
	}

	inline void bc_xbot_idle_actor_controller::update_replicated_instance(const game::bc_actor_component_update_content& p_context)
	{
	}

	inline void bc_xbot_idle_actor_controller::throw_grenade(game::bc_actor& p_grenade) noexcept
	{
	}
}