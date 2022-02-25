// [02/24/2022 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcTimingComponent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"

namespace black_cat
{
	namespace game
	{
		bc_timing_component::bc_timing_component(bc_actor_id p_actor_index, bc_actor_component_id p_index)
			: bci_actor_component(p_actor_index, p_index)
		{
		}

		bc_actor bc_timing_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_timing_component::set_scene_add_callback(callback p_callback) noexcept
		{
			m_scene_add_callback = std::move(p_callback);
		}

		void bc_timing_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			if(core::bci_message::is<bc_added_to_scene_actor_event>(p_context.m_event) && m_scene_add_callback.is_valid())
			{
				// Run callback by another event to let all components receive 'added to scene' event
				p_context.m_actor.add_event(_bc_run_scene_callback_actor_event());
				return;
			}

			if(core::bci_message::is<_bc_run_scene_callback_actor_event>(p_context.m_event))
			{
				m_scene_add_callback(p_context.m_actor);
				m_scene_add_callback.reset();
				return;
			}
		}
	}
}
