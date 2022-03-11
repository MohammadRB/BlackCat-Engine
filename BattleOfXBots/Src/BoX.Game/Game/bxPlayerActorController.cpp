// [12/23/2021 MRB]

#include "Game/System/Network/bcNetworkSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/Component/bcHumanRagdollComponent.h"
#include "Game/bcJsonParse.h"
#include "App/SampleImp/XBot/bcXBotRagdollNetworkMessage.h"
#include "BoX.Game/Application/bxPlayerService.h"
#include "BoX.Game/Game/bxPlayerActorController.h"
#include "BoX.Game/bxEvent.h"

namespace box
{
	bx_player_actor_controller::bx_player_actor_controller() noexcept
		: m_event_manager(core::bc_get_service<core::bc_event_manager>()),
		m_ui_service(core::bc_get_service<bx_player_service>()),
		m_rifle_heat_per_shoot(5),
		m_rifle_cool_per_second(10),
		m_grenade_load_time(30),
		m_smoke_load_time(30),
		m_network_health(100),
		m_rifle_heat(0),
		m_grenade_load(100),
		m_smoke_load(100)
	{
	}

	void bx_player_actor_controller::killed(game::bc_network_client_id p_killer_id, core::bc_string_view p_body_part_force, const core::bc_vector3f& p_force) noexcept
	{
		bc_xbot_actor_controller::enable_ragdoll(p_body_part_force, p_force);

		bx_player_killed_event l_event(p_killer_id, get_actor());
		m_event_manager->process_event(l_event);
	}

	void bx_player_actor_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		bc_xbot_player_actor_controller::initialize(p_context);
	}

	void bx_player_actor_controller::added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		bc_xbot_player_actor_controller::added_to_scene(p_context, p_scene);

		const auto* l_mediate_component = p_context.m_actor.get_component<game::bc_mediate_component>();
		const auto l_bound_box_extends = l_mediate_component->get_bound_box().get_half_extends();
		const auto l_max_side_length = std::max(std::max(l_bound_box_extends.x, l_bound_box_extends.y), l_bound_box_extends.z) * 2;
		const auto l_camera_y_offset = l_max_side_length * 3.5f;
		const auto l_camera_z_offset = l_max_side_length * -1.5f;
		const auto l_camera_look_at_offset = l_max_side_length * 1.25f;

		set_camera_offsets(l_camera_y_offset, l_camera_z_offset, l_camera_look_at_offset);

		bx_player_spawned_event l_event(p_context.m_actor);
		m_event_manager->process_event(l_event);
	}

	void bx_player_actor_controller::load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
		bc_xbot_player_actor_controller::load_origin_network_instance(p_context);
		json_parse::bc_load(p_context.m_parameters, "hth", m_network_health);
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

		m_rifle_heat -= p_context.m_clock.m_elapsed_second * m_rifle_cool_per_second;
		m_grenade_load += p_context.m_clock.m_elapsed_second * (100.f / m_grenade_load_time);
		m_smoke_load += p_context.m_clock.m_elapsed_second * (100.f / m_smoke_load_time);

		m_rifle_heat = std::max(m_rifle_heat, 0.f);
		m_grenade_load = std::min(m_grenade_load, 100.f);
		m_smoke_load = std::min(m_smoke_load, 100.f);

		m_ui_service->set_health(m_network_health);
		m_ui_service->set_weapon_heat(m_rifle_heat);
		m_ui_service->set_grenade_load(m_grenade_load);
		m_ui_service->set_smoke_load(m_smoke_load);
	}

	void bx_player_actor_controller::update_replicated_instance(const game::bc_actor_component_update_content& p_context)
	{
		bc_xbot_player_actor_controller::update_replicated_instance(p_context);
	}

	void bx_player_actor_controller::removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		bc_xbot_player_actor_controller::removed_from_scene(p_context, p_scene);

		bx_player_removed_event l_event(p_context.m_actor);
		m_event_manager->process_event(l_event);
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
		return m_network_health > 0;
	}

	bool bx_player_actor_controller::can_throw_grenade(core::bc_string_view p_entity) noexcept
	{
		if (p_entity.find("smoke") != std::string::npos)
		{
			return m_network_health > 0 && m_smoke_load == 100;
		}
		else
		{
			return m_network_health > 0 && m_grenade_load == 100;
		}
	}

	bool bx_player_actor_controller::can_attach_weapon(core::bc_string_view p_entity) noexcept
	{
		return true;
	}

	bool bx_player_actor_controller::can_shoot_weapon() noexcept
	{
		return m_network_health > 0 && m_rifle_heat < 95;
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
		m_rifle_heat = std::min(m_rifle_heat, 100.f);
	}
}
