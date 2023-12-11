// [14/10/2023 MRB]

#include "Game/GamePCH.h"

#include "Game/Object/Scene/Component/bcWaterComponent.h"
#include "Game/Object/Scene/Component/bcIconComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/bcJsonParse.h"
#include "Game/bcConstant.h"

namespace black_cat::game
{
	bc_water_component::bc_water_component(bc_actor_id p_actor_id, bc_actor_component_id p_id)
		: bc_base_actor_component(p_actor_id, p_id)
	{
	}

	void bc_water_component::initialize_entity(const bc_actor_component_initialize_entity_context& p_context)
	{
		auto* l_icon_component = p_context.m_actor.get_create_component<bc_icon_component>();
		l_icon_component->set_icon(bc_icon_type::fog);
	}

	void bc_water_component::load_instance(const bc_actor_component_load_context& p_context)
	{
		json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_global_half_extend, m_half_extend);
		json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_global_color, m_color);
	}

	void bc_water_component::write_instance(const bc_actor_component_write_context& p_context)
	{
		json_parse::bc_write(p_context.m_parameters, constant::g_param_global_half_extend, m_half_extend);
		json_parse::bc_write(p_context.m_parameters, constant::g_param_global_color, m_color);
	}
}
