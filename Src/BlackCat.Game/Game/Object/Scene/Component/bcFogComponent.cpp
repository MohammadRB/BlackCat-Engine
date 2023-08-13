// [23/07/2023 MRB]

#include "Game/GamePCH.h"

#include "Game/Object/Scene/Component/bcFogComponent.h"
#include "Game/Object/Scene/Component/bcIconComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/bcJsonParse.h"
#include "Game/bcConstant.h"

namespace black_cat::game
{
	bc_fog_component::bc_fog_component(bc_actor_id p_actor_id, bc_actor_component_id p_id)
		: bci_actor_component(p_actor_id, p_id),
		m_center(0),
		m_extend(.5)
	{
	}

	bc_actor bc_fog_component::get_actor() const noexcept
	{
		return get_manager().component_get_actor<bc_fog_component>(*this);
	}

	void bc_fog_component::initialize_entity(const bc_actor_component_initialize_entity_context& p_context)
	{
		auto* l_icon_component = p_context.m_actor.get_create_component<bc_icon_component>();
		l_icon_component->set_icon(bc_icon_type::fog);
	}

	void bc_fog_component::load_instance(const bc_actor_component_load_context& p_context)
	{
		json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_fog_center, m_center);
		json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_fog_extend, m_extend);
	}

	void bc_fog_component::write_instance(const bc_actor_component_write_context& p_context)
	{
		json_parse::bc_write(p_context.m_parameters, constant::g_param_fog_center, m_center);
		json_parse::bc_write(p_context.m_parameters, constant::g_param_fog_extend, m_extend);
	}

	void bc_fog_component::handle_event(const bc_actor_component_event_context& p_context)
	{
		if (const auto* l_transform_event = core::bci_event::as<bc_world_transform_actor_event>(p_context.m_event))
		{
			m_center = l_transform_event->get_transform().get_translation();
		}
	}
}
