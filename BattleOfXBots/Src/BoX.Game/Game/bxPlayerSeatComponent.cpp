// [09/02/2022 MRB]

#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcIconComponent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/bcJsonParse.h"
#include "App/bcException.h"
#include "BoX.Game/Game/bxPlayerSeatComponent.h"

namespace box
{
	bx_player_seat_component::bx_player_seat_component(game::bc_actor_id p_actor_index, game::bc_actor_component_id p_index)
		: bci_actor_component(p_actor_index, p_index),
		m_team(bx_team::red)
	{
	}
	
	void bx_player_seat_component::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		const core::bc_string* l_team_value = nullptr;
		json_parse::bc_load_throw(p_context.m_parameters, g_param_team, l_team_value);

		if(*l_team_value == "red")
		{
			m_team = bx_team::red;
		}
		else if(*l_team_value == "blue")
		{
			m_team = bx_team::blue;
		}
		else
		{
			throw bc_invalid_argument_exception("invalid team parameter");
		}

		auto* l_icon_component = p_context.m_actor.get_create_component<game::bc_icon_component>();
		l_icon_component->set_icon("player");
	}

	void bx_player_seat_component::handle_event(const game::bc_actor_component_event_context& p_context)
	{
		if (const auto* l_world_transform_event = core::bci_message::as<game::bc_world_transform_actor_event>(p_context.m_event))
		{
			//const auto& l_transform = l_world_transform_event->get_transform();
			//const auto& l_bound_box = physics::bc_bound_box(l_transform.get_translation(), core::bc_vector3f(.5f));
			//p_context.m_actor.add_event(game::bc_bound_box_changed_actor_event(l_bound_box));

			return;
		}
	}
}
