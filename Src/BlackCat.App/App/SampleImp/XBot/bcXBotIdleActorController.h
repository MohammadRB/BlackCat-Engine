// [10/16/2021 MRB]

#pragma once

#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/Object/Scene/Component/bcHumanRagdollComponent.h"
#include "Game/Object/Scene/Component/bcRigidControllerComponent.h"
#include "Game/Object/Scene/Component/Event/bcBulletHitActorEvent.h"
#include "App/SampleImp/XBot/bcXBotActorController.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_xbot_idle_actor_controller : public bc_xbot_actor_controller
	{
	private:
		void initialize(const game::bc_actor_component_initialize_context& p_context) override;
		
		void load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context) override;
		
		void load_replicated_network_instance(const game::bc_actor_component_network_load_context& p_context) override;
		
		void write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context) override;
		
		void write_replicated_network_instance(const game::bc_actor_component_network_write_context& p_context) override;
		
		void update_origin_instance(const game::bc_actor_component_update_content& p_context) override;
		
		void update_replicated_instance(const game::bc_actor_component_update_content& p_context) override;

		void handle_event(const game::bc_actor_component_event_context& p_context) override;

		void throw_grenade(game::bc_actor& p_grenade) noexcept override;

		game::bc_physics_system* m_physics_system = nullptr;
		game::bc_human_ragdoll_component* m_ragdoll_component = nullptr;
		game::bc_rigid_controller_component* m_rigid_controller_component = nullptr;
		bool m_ragdoll_enabled = false;
	};

	inline void bc_xbot_idle_actor_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		bc_xbot_actor_controller::initialize(p_context);

		m_physics_system = &p_context.m_game_system.get_physics_system();
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
		if(m_ragdoll_enabled || p_context.m_is_double_update)
		{
			return;
		}
		
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

	inline void bc_xbot_idle_actor_controller::update_replicated_instance(const game::bc_actor_component_update_content& p_context)
	{
	}

	inline void bc_xbot_idle_actor_controller::handle_event(const game::bc_actor_component_event_context& p_context)
	{
		bc_xbot_actor_controller::handle_event(p_context);
		
		const auto* l_bullet_hit_event = core::bci_message::as<game::bc_bullet_hit_actor_event>(p_context.m_event);
		if (l_bullet_hit_event)
		{
			enable_ragdoll();
			m_ragdoll_component->handle_event(p_context);
			m_ragdoll_enabled = true;
		}
	}

	inline void bc_xbot_idle_actor_controller::throw_grenade(game::bc_actor& p_grenade) noexcept
	{
	}
}