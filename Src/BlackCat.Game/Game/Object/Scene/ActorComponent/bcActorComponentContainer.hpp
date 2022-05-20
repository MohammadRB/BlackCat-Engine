// [06/17/2021 MRB]

#pragma once

// This file has to be to break dependency between bcActorComponentContainer and bcActorComponentManagerContainer
#include "Game/Object/Scene/ActorComponent/bcActorComponentContainer.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"

namespace black_cat
{
	namespace game
	{
		template<class TComponent>
		core::bc_vector_frame<bcUINT32> bc_actor_component_container<TComponent>::export_alive_indices() const
		{
			return m_bit_block.find_true_indices();
		}

		template<class TComponent>
		void bc_actor_component_container<TComponent>::handle_events(const platform::bc_clock::update_param& p_clock,
			core::bc_query_manager& p_query_manager,
			bc_game_system& p_game_system,
			bc_actor_component_manager_container& p_manager,
			const core::bc_vector_frame<bcUINT32>& p_alive_components)
		{
			for (auto l_index : p_alive_components)
			{
				TComponent& l_component = m_components[l_index];
				bc_actor l_actor = p_manager.component_get_actor<TComponent>(l_component);
				bc_actor_event* l_events = p_manager.actor_get_events(l_actor);
				bc_actor_event* l_current_event = l_events;
				bcUINT32 l_event_index = 0;

				while (l_current_event)
				{
					l_component.handle_event(bc_actor_component_event_context(p_clock, p_query_manager, p_game_system, l_actor, *l_current_event, l_event_index));
					l_current_event = l_current_event->get_next();
					l_event_index++;
				}
			}
		}

		template<class TComponent>
		void bc_actor_component_container<TComponent>::update(const platform::bc_clock::update_param& p_clock,
			core::bc_query_manager& p_query_manager,
			bc_game_system& p_game_system,
			bc_actor_component_manager_container& p_manager,
			const core::bc_vector_frame<bcUINT32>& p_alive_components)
		{
			for (auto l_index : p_alive_components)
			{
				TComponent& l_component = m_components[l_index];
				bc_actor l_actor = p_manager.component_get_actor<TComponent>(l_component);

				l_component.update(bc_actor_component_update_content(p_clock, p_query_manager, p_game_system, l_actor));
			}
		}
	}	
}