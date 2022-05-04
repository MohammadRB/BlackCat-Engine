// [01/01/2022 MRB]

#include "Core/Messaging/Event/bcEventManager.h"
#include "Game/System/Network/bcNetworkSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcCallbackComponent.h"
#include "Game/Object/Scene/Component/Event/bcExplosionActorEvent.h"
#include "Game/Object/Scene/bcScene.h"
#include "BoX.Game/Game/bxNetworkPlayerActorController.h"
#include "BoX.Game/Network/bxPlayerKilledNetworkMessage.h"
#include "BoX.Game/bxEvent.h"

namespace box
{
	bx_network_player_actor_controller::bx_network_player_actor_controller() noexcept
		: m_event_manager(core::bc_get_service<core::bc_event_manager>()),
		m_player_service(core::bc_get_service<bxi_player_list_service>()),
		m_health_recover_per_second(10),
		m_health_damage_per_thousands_force(8),
		m_health(100)
	{
	}

	void bx_network_player_actor_controller::killed(game::bc_network_client_id p_killer_id, core::bc_string_view p_body_part_force, const core::bc_vector3f& p_force) noexcept
	{
		if (auto* l_weapon = get_weapon())
		{
			auto* l_callback_component = l_weapon->m_actor.get_create_component<game::bc_callback_component>();
			l_callback_component->set_as_auto_remove(8000);
		}

		bc_xbot_actor_controller::enable_ragdoll(p_body_part_force, p_force);

		bx_player_killed_event l_event(p_killer_id, get_network_client_id(), get_actor());
		m_event_manager->process_event(l_event);
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
		p_context.m_parameters.add_or_update("hth", core::bc_any(m_health));
	}

	void bx_network_player_actor_controller::added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		bc_xbot_network_player_actor_controller::added_to_scene(p_context, p_scene);

		bx_player_spawned_event l_event(get_network_client_id(), p_context.m_actor);
		m_event_manager->process_event(l_event);
	}

	void bx_network_player_actor_controller::update_origin_instance(const game::bc_actor_component_update_content& p_context)
	{
		bc_xbot_network_player_actor_controller::update_origin_instance(p_context);
	}

	void bx_network_player_actor_controller::update_replicated_instance(const game::bc_actor_component_update_content& p_context)
	{
		bc_xbot_network_player_actor_controller::update_replicated_instance(p_context);

		if(get_network_type() == game::bc_network_type::server)
		{
			m_health = m_health > 0.f ? m_health + p_context.m_clock.m_elapsed_second * m_health_recover_per_second : m_health;
			m_health = std::min(m_health, 100.f);
		}
	}

	void bx_network_player_actor_controller::removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		bc_xbot_network_player_actor_controller::removed_from_scene(p_context, p_scene);

		bx_player_removed_event l_event(get_network_client_id(), p_context.m_actor);
		m_event_manager->process_event(l_event);
	}

	void bx_network_player_actor_controller::handle_event(const game::bc_actor_component_event_context& p_context)
	{
		bc_xbot_network_player_actor_controller::handle_event(p_context);
		
		if(get_network_type() == game::bc_network_type::server && m_health > 0)
		{
			bool l_health_hit = false;
			const auto l_client_id = get_network_client_id();
			auto l_hit_client_id = game::g_invalid_actor_network_id;

			if (const auto* l_bullet_hit_event = core::bci_message::as<game::bc_bullet_hit_actor_event>(p_context.m_event))
			{
				l_hit_client_id = l_bullet_hit_event->get_bullet_player_id();

				const auto l_are_in_same_team = m_player_service->are_in_same_team(l_client_id, l_hit_client_id);
				if (!l_are_in_same_team)
				{
					const auto l_force = l_bullet_hit_event->calculate_applied_force();
					m_health -= l_force / 1000.f * static_cast<bcFLOAT>(m_health_damage_per_thousands_force);
					l_health_hit = true;
				}
			}

			if (const auto* l_explosion_event = core::bci_message::as<game::bc_explosion_actor_event>(p_context.m_event))
			{
				l_hit_client_id = l_explosion_event->get_player_id();

				const auto l_are_in_same_team = m_player_service->are_in_same_team(l_client_id, l_hit_client_id);
				if (!l_are_in_same_team)
				{
					const auto l_force = l_explosion_event->calculate_applied_force(get_position());
					m_health -= l_force.second / 1000.f * static_cast<bcFLOAT>(m_health_damage_per_thousands_force);
					l_health_hit = true;
				}
			}

			if (l_health_hit && m_health <= 0)
			{
				if (auto* l_weapon = get_weapon())
				{
					auto* l_callback_component = l_weapon->m_actor.get_create_component<game::bc_callback_component>();
					l_callback_component->set_as_auto_remove(8000);
				}

				bc_xbot_actor_controller::enable_ragdoll();

				// Let ragdoll component add needed force in case if has received the event before controller
				auto* l_ragdoll_component = p_context.m_actor.get_component<game::bc_human_ragdoll_component>();
				l_ragdoll_component->handle_event(p_context);

				auto [l_body_part, l_force] = l_ragdoll_component->get_last_applied_force();
				p_context.m_game_system.get_network_system().send_message(bx_player_killed_network_message(l_hit_client_id, get_actor(), l_body_part.data(), l_force));

				bx_player_killed_event l_event(l_hit_client_id, l_client_id, p_context.m_actor);
				m_event_manager->process_event(l_event);
			}
		}
	}
}
