// [11/05/2021 MRB]

#include "Core/Utility/bcJsonParse.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/bcJsonParse.h"
#include "Game/bcUtility.h"
#include "BoX.Game/Game/bxGrenadeActorController.h"

namespace box
{
	void bx_grenade_actor_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		bc_rigid_dynamic_network_actor_controller::initialize(p_context);

		const core::bc_string* l_explosion_entity_name;

		json_parse::bc_load_throw(p_context.m_parameters, "explosion_entity", l_explosion_entity_name);
		json_parse::bc_load_throw(p_context.m_parameters, "lifetime", m_lifetime);
		json_parse::bc_load(p_context.m_instance_parameters, constant::g_param_player_id, m_player_id);

		m_explosion_entity_name = l_explosion_entity_name->c_str();
	}

	void bx_grenade_actor_controller::update_origin_instance(const game::bc_actor_component_update_content& p_context)
	{
		bc_rigid_dynamic_network_actor_controller::update_origin_instance(p_context);
		update_replicated_instance(p_context);

		/*if(get_network_type() == game::bc_network_type::not_started)
		{
			update_replicated_instance(p_context);
			return;
		}
		
		if(!get_scene())
		{
			return;
		}

		m_lifetime -= p_context.m_clock.m_elapsed_second;
		if(m_lifetime <= 0)
		{
			get_scene()->remove_actor(p_context.m_actor);
		}*/
	}

	void bx_grenade_actor_controller::update_replicated_instance(const game::bc_actor_component_update_content& p_context)
	{
		bc_rigid_dynamic_network_actor_controller::update_replicated_instance(p_context);

		if (!get_scene() || p_context.m_is_double_update || m_exploded)
		{
			return;
		}

		const auto l_ping = get_in_ping() / 1000.0f;
		m_lifetime -= p_context.m_clock.m_elapsed_second;
		if (m_lifetime - l_ping <= 0)
		{
			// actor will be removed by its network component if network has started
			if (get_network_type() == game::bc_network_type::not_started || get_network_type() == game::bc_network_type::server)
			{
				get_scene()->remove_actor(p_context.m_actor);
			}
		}
	}

	void bx_grenade_actor_controller::removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		bc_rigid_dynamic_network_actor_controller::removed_from_scene(p_context, p_scene);
		_explode(p_context.m_actor);
	}

	void bx_grenade_actor_controller::_explode(game::bc_actor& p_actor)
	{
		const auto* l_mediate_component = p_actor.get_component<game::bc_mediate_component>();

		core::bc_json_key_value l_instance_params;
		l_instance_params.add_or_update(constant::g_param_player_id, core::bc_any(m_player_id));

		auto l_explosion_actor = get_scene()->create_actor
		(
			m_explosion_entity_name,
			bc_matrix4f_from_position_and_direction(l_mediate_component->get_position(), core::bc_vector3f::up()),
			l_instance_params
		);
		l_explosion_actor.mark_for_double_update();

		m_exploded = true;
	}
}
