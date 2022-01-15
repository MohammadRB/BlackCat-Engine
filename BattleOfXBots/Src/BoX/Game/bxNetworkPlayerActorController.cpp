// [01/01/2022 MRB]

#include "Game/Object/Scene/Component/Event/bcExplosionActorEvent.h"
#include "App/SampleImp/XBot/bcXBotRagdollNetworkMessage.h"
#include "BoX/Game/bxNetworkPlayerActorController.h"

namespace box
{
	bx_network_player_actor_controller::bx_network_player_actor_controller() noexcept
		: m_health_recover_per_second(10),
		m_health_damage_per_thousands_force(8),
		m_health(100)
	{
	}

	void bx_network_player_actor_controller::load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
		bc_xbot_network_player_actor_controller::load_origin_network_instance(p_context);
	}

	void bx_network_player_actor_controller::load_replicated_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
		bc_xbot_network_player_actor_controller::load_replicated_network_instance(p_context);
	}

	void bx_network_player_actor_controller::write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
		bc_xbot_network_player_actor_controller::write_origin_network_instance(p_context);
	}

	void bx_network_player_actor_controller::write_replicated_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
		bc_xbot_network_player_actor_controller::write_replicated_network_instance(p_context);
		p_context.m_parameters.add("hth", core::bc_any(m_health));
	}

	void bx_network_player_actor_controller::added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		bc_xbot_network_player_actor_controller::added_to_scene(p_context, p_scene);
	}

	void bx_network_player_actor_controller::update_origin_instance(const game::bc_actor_component_update_content& p_context)
	{
		bc_xbot_network_player_actor_controller::update_origin_instance(p_context);
	}

	void bx_network_player_actor_controller::update_replicated_instance(const game::bc_actor_component_update_content& p_context)
	{
		bc_xbot_network_player_actor_controller::update_replicated_instance(p_context);

		m_health = m_health > 0.f ? m_health + p_context.m_clock.m_elapsed_second * m_health_recover_per_second : m_health;
		m_health = std::min(m_health, 100.f);
	}

	void bx_network_player_actor_controller::removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		bc_xbot_network_player_actor_controller::removed_from_scene(p_context, p_scene);
	}

	void bx_network_player_actor_controller::handle_event(const game::bc_actor_component_event_context& p_context)
	{
		bc_xbot_network_player_actor_controller::handle_event(p_context);

		bool l_health_hit = false;

		const auto* l_bullet_hit_event = core::bci_message::as<game::bc_bullet_hit_actor_event>(p_context.m_event);
		if (l_bullet_hit_event)
		{
			const auto l_force = l_bullet_hit_event->calculate_applied_force();
			m_health -= l_force / 1000.f * m_health_damage_per_thousands_force;
			l_health_hit = true;
		}

		const auto* l_explosion_event = core::bci_message::as<game::bc_explosion_actor_event>(p_context.m_event);
		if (l_explosion_event)
		{
			const auto l_force = l_explosion_event->calculate_applied_force(get_position());
			m_health -= l_force.second / 1000.f * m_health_damage_per_thousands_force;
			l_health_hit = true;
		}

		if (l_health_hit && m_health <= 0)
		{
			bc_xbot_actor_controller::enable_ragdoll();

			// Let ragdoll component add needed force in case if has received the event before controller
			auto* l_ragdoll_component = p_context.m_actor.get_component<game::bc_human_ragdoll_component>();
			l_ragdoll_component->handle_event(p_context);

			auto [l_body_part, l_force] = l_ragdoll_component->get_last_applied_force();
			p_context.m_game_system.get_network_system().send_message(bc_xbot_ragdoll_activation_network_message(get_actor(), l_body_part.data(), l_force));
		}
	}
}
