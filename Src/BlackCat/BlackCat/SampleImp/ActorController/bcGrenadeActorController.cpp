// [11/05/2021 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Utility/bcJsonParse.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/bcUtility.h"
#include "BlackCat/SampleImp/ActorController/bcGrenadeActorController.h"

namespace black_cat
{
	void bc_grenade_actor_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		bc_rigid_dynamic_network_actor_controller::initialize(p_context);
		
		m_explosion_entity_name = p_context.m_parameters.get_value_throw<core::bc_string>("explosion_entity").c_str();
		m_lifetime = p_context.m_parameters.get_value_throw<bcFLOAT>("lifetime");
	}

	void bc_grenade_actor_controller::update_origin_instance(const game::bc_actor_component_update_content& p_context)
	{
		if(get_network_type() == game::bc_network_type::not_started)
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
		}
	}

	void bc_grenade_actor_controller::update_replicated_instance(const game::bc_actor_component_update_content& p_context)
	{
		if (!get_scene())
		{
			return;
		}

		const auto l_ping = get_in_ping() / 1000.0f;
		m_lifetime -= p_context.m_clock.m_elapsed_second;
		if (m_lifetime - l_ping <= 0)
		{
			const auto* l_mediate_component = p_context.m_actor.get_component<game::bc_mediate_component>();
			auto l_explosion_actor = get_scene()->create_actor
			(
				m_explosion_entity_name,
				bc_matrix4f_from_position_and_direction(l_mediate_component->get_position(), core::bc_vector3f::up())
			);
			l_explosion_actor.mark_for_double_update();

			if (get_network_type() == game::bc_network_type::not_started)
			{
				get_scene()->remove_actor(p_context.m_actor);
			}

			m_lifetime = std::numeric_limits<bcFLOAT>::max();
		}
	}

	void bc_grenade_actor_controller::handle_event(const game::bc_actor_component_event_context& p_context)
	{
	}
}