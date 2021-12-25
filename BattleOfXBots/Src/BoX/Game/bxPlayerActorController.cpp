// [12/23/2021 MRB]

#include "Game/System/Network/bcNetworkSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/Component/Event/bcBulletHitActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcExplosionActorEvent.h"
#include "BoX/Application/bxPlayerUIService.h"
#include "BoX/Game/bxPlayerActorController.h"

namespace box
{
	bx_player_actor_controller::bx_player_actor_controller() noexcept
		: m_ui_service(core::bc_get_service<bx_player_ui_service>()),
		m_health_recover_per_second(20),
		m_rifle_heat_per_shoot(3),
		m_rifle_cool_per_second(20),
		m_grenade_load_time(30),
		m_smoke_load_time(30),
		m_health(100),
		m_rifle_heat(0),
		m_grenade_load(0),
		m_smoke_load(0)
	{
	}

	void bx_player_actor_controller::added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		bc_xbot_player_actor_controller::added_to_scene(p_context, p_scene);
	}

	void bx_player_actor_controller::load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
		bc_xbot_player_actor_controller::load_origin_network_instance(p_context);
	}

	void bx_player_actor_controller::load_replicated_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
		bc_xbot_player_actor_controller::load_replicated_network_instance(p_context);
	}

	void bx_player_actor_controller::write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
		bc_xbot_player_actor_controller::write_origin_network_instance(p_context);
	}

	void bx_player_actor_controller::write_replicated_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
		bc_xbot_player_actor_controller::write_replicated_network_instance(p_context);
	}

	void bx_player_actor_controller::update_origin_instance(const game::bc_actor_component_update_content& p_context)
	{
		bc_xbot_player_actor_controller::update_origin_instance(p_context);

		m_health += p_context.m_clock.m_elapsed_second * m_health_recover_per_second;
		m_rifle_heat -= p_context.m_clock.m_elapsed_second * m_rifle_cool_per_second;
		m_grenade_load += p_context.m_clock.m_elapsed_second * m_grenade_load_time;
		m_smoke_load += p_context.m_clock.m_elapsed_second * m_smoke_load_time;

		m_health = std::max(m_health, 100.f);
		m_rifle_heat = std::min(m_rifle_heat, 0.f);
		m_grenade_load = std::max(m_grenade_load, 100.f);
		m_smoke_load = std::max(m_smoke_load, 100.f);
	}

	void bx_player_actor_controller::update_replicated_instance(const game::bc_actor_component_update_content& p_context)
	{
		bc_xbot_player_actor_controller::update_replicated_instance(p_context);
	}

	void bx_player_actor_controller::removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		bc_xbot_player_actor_controller::removed_from_scene(p_context, p_scene);
	}

	void bx_player_actor_controller::handle_event(const game::bc_actor_component_event_context& p_context)
	{
		bc_xbot_player_actor_controller::handle_event(p_context);
	}

	bool bx_player_actor_controller::can_look() noexcept
	{
		return true;
	}

	bool bx_player_actor_controller::can_move() noexcept
	{
		return m_health > 0;
	}

	bool bx_player_actor_controller::can_throw_grenade(core::bc_string_view p_entity) noexcept
	{
		return m_health > 0 && m_grenade_load == 100;
	}

	bool bx_player_actor_controller::can_attach_weapon(core::bc_string_view p_entity) noexcept
	{
		return true;
	}

	bool bx_player_actor_controller::can_shoot_weapon() noexcept
	{
		return m_health > 0 && m_rifle_heat < 100;
	}

	void bx_player_actor_controller::grenade_thrown(core::bc_string_view p_entity) noexcept
	{
		if(p_entity.find("smoke") != std::string::npos)
		{
			m_smoke_load = 0;
		}
		else
		{
			m_grenade_load = 0;
		}
	}

	void bx_player_actor_controller::weapon_attached(game::bc_actor& p_weapon) noexcept
	{
	}

	void bx_player_actor_controller::weapon_shoot(game::bc_actor& p_weapon) noexcept
	{
		m_rifle_heat += m_rifle_heat_per_shoot;
		m_rifle_heat = std::max(m_rifle_heat, 100.f);
	}
}