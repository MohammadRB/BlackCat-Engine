// [06/17/2021 MRB]

#pragma once

// This file is to breaking dependency between bcActorComponentContainer and bcActorComponentManager
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentContainer.h"

namespace black_cat
{
	namespace game
	{
		template<class TComponent>
		void bc_actor_component_container<TComponent>::handle_events(core::bc_query_manager& p_query_manager,
			bc_game_system& p_game_system,
			bc_actor_component_manager& p_manager)
		{
			const auto l_used_slots = m_bit_block.find_true_indices();

			for (auto l_index : l_used_slots)
			{
				TComponent& l_component = m_components[l_index];
				bc_actor l_actor = p_manager.component_get_actor<TComponent>(l_component);
				bc_actor_event* l_events = p_manager.actor_get_events(l_actor);
				bc_actor_event* l_current_event = l_events;

				while (l_current_event)
				{
					l_component.handle_event(bc_actor_component_event_context(p_query_manager, p_game_system, l_actor, *l_current_event));
					l_current_event = l_current_event->get_next();
				}
			}
		}

		template<class TComponent>
		void bc_actor_component_container<TComponent>::update(core::bc_query_manager& p_query_manager,
			bc_game_system& p_game_system,
			bc_actor_component_manager& p_manager,
			const platform::bc_clock::update_param& p_clock)
		{
			const auto l_used_slots = m_bit_block.find_true_indices();

			for (auto l_index : l_used_slots)
			{
				TComponent& l_component = m_components[l_index];
				bc_actor l_actor = p_manager.component_get_actor<TComponent>(l_component);

				l_component.update(bc_actor_component_update_content(p_clock, p_query_manager, p_game_system, l_actor));
			}
		}
	}	
}