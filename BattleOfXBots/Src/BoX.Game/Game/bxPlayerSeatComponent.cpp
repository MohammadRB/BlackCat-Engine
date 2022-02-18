// [02/09/2022 MRB]

#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcIconComponent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "App/bcException.h"
#include "BoX.Game/Game/bxPlayerSeatComponent.h"

namespace box
{
	bx_player_seat_component::bx_player_seat_component(game::bc_actor_id p_actor_index, game::bc_actor_component_id p_index)
		: bci_actor_component(p_actor_index, p_index),
		m_team(bx_team::red)
	{
	}

	game::bc_actor bx_player_seat_component::get_actor() const noexcept
	{
		return get_manager().component_get_actor(*this);
	}

	void bx_player_seat_component::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		const auto& l_team_value = p_context.m_parameters.get_value_throw<core::bc_string>(g_param_team);

		if(l_team_value == "red")
		{
			m_team = bx_team::red;
		}
		else if(l_team_value == "blue")
		{
			m_team = bx_team::blue;
		}
		else
		{
			throw bc_invalid_argument_exception("invalid team parameter");
		}

		const auto l_icon_parameters = core::bc_data_driven_parameter(core::bc_alloc_type::frame)
			.add_or_update(constant::g_param_icon_name, core::bc_string("player"));
		p_context.m_actor.create_component<game::bc_icon_component>(l_icon_parameters);
	}

	void bx_player_seat_component::handle_event(const game::bc_actor_component_event_context& p_context)
	{
		if (const auto* l_world_transform_event = core::bci_message::as<game::bc_world_transform_actor_event>(p_context.m_event))
		{
			const auto& l_transform = l_world_transform_event->get_transform();
			const auto& l_bound_box = physics::bc_bound_box(l_transform.get_translation(), core::bc_vector3f(.5f));
			p_context.m_actor.add_event(game::bc_bound_box_changed_actor_event(l_bound_box));

			return;
		}
	}
}
