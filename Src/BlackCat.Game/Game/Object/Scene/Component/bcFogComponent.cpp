// [23/07/2023 MRB]

#include "Game/GamePCH.h"

#include "Game/Object/Scene/Component/bcFogComponent.h"
#include "Game/Object/Scene/Component/bcIconComponent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/bcJsonParse.h"
#include "Game/bcConstant.h"

namespace black_cat::game
{
	bc_fog_component::bc_fog_component(bc_actor_id p_actor_id, bc_actor_component_id p_id)
		: bci_actor_component(p_actor_id, p_id),
		  m_half_extend(.5),
		  m_color(1),
		  m_visibility(0),
		  m_center_fade(.5),
		  m_intensity(0),
		  m_global_light(false)
	{
	}
	
	core::bc_vector3f bc_fog_component::get_center() const noexcept
	{
		const auto* l_mediate_component = get_actor().get_component<bc_mediate_component>();
		return l_mediate_component->get_position();
	}

	void bc_fog_component::initialize_entity(const bc_actor_component_initialize_entity_context& p_context)
	{
		auto* l_icon_component = p_context.m_actor.get_create_component<bc_icon_component>();
		l_icon_component->set_icon(bc_icon_type::fog);
	}

	void bc_fog_component::load_instance(const bc_actor_component_load_context& p_context)
	{
		json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_fog_half_extend, m_half_extend);
		json_parse::bc_load(p_context.m_parameters, constant::g_param_fog_color, m_color);
		json_parse::bc_load(p_context.m_parameters, constant::g_param_fog_center_fade, m_center_fade);
		json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_fog_visibility, m_visibility);
		json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_fog_intensity, m_intensity);
		json_parse::bc_load(p_context.m_parameters, constant::g_param_fog_global_light, m_global_light);
	}

	void bc_fog_component::write_instance(const bc_actor_component_write_context& p_context)
	{
		json_parse::bc_write(p_context.m_parameters, constant::g_param_fog_half_extend, m_half_extend);
		json_parse::bc_write(p_context.m_parameters, constant::g_param_fog_color, m_color);
		json_parse::bc_write(p_context.m_parameters, constant::g_param_fog_center_fade, m_center_fade);
		json_parse::bc_write(p_context.m_parameters, constant::g_param_fog_visibility, m_visibility);
		json_parse::bc_write(p_context.m_parameters, constant::g_param_fog_intensity, m_intensity);
		json_parse::bc_write(p_context.m_parameters, constant::g_param_fog_global_light, m_global_light);
	}
}