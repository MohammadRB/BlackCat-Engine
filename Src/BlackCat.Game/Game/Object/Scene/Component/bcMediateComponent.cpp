// [12/22/2018 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcJsonParse.h"
#include "Core/Utility/bcLogger.h"
#include "Game/System/Render/bcShapeDrawer.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"
#include "Game/Object/Scene/bcScene.h"

namespace black_cat
{
	namespace game
	{
		bc_mediate_component::bc_mediate_component(bc_actor_id p_actor_id, bc_actor_component_id p_id)
			: bci_actor_component(p_actor_id, p_id),
			m_entity_name(nullptr),
			m_added_to_scene(false),
			m_scene(nullptr),
			m_last_bound_box_change_time(0),
			m_world_transform()
		{
		}

		bc_actor bc_mediate_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_mediate_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			m_scene = &p_context.m_scene;
			m_prev_bound_box = m_bound_box = physics::bc_bound_box({ 0, 0, 0 }, { .5f, .5f, .5f }); // Assign default bound box
		}
		
		void bc_mediate_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			if (const auto* l_transformation_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event))
			{
				m_world_transform = l_transformation_event->get_transform();

				// update prev box only once in case of multiple events per frame
				if (m_last_bound_box_change_time < p_context.m_clock.m_total_elapsed)
				{
					m_prev_bound_box = m_bound_box;
					m_last_bound_box_change_time = static_cast<platform::bc_clock::small_time>(p_context.m_clock.m_total_elapsed) + 1.f; // +1ms to avoid floating point precision problem

					if (m_added_to_scene)
					{
						m_scene->update_actor(p_context.m_actor);
					}
				}
				m_bound_box = physics::bc_bound_box(m_world_transform.get_translation(), m_bound_box.get_half_extends());
				return;
			}

			if (const auto* l_bound_box_event = core::bci_message::as<bc_bound_box_changed_actor_event>(p_context.m_event))
			{
				// update prev box only once in case of multiple events per frame
				if (m_last_bound_box_change_time < p_context.m_clock.m_total_elapsed)
				{
					m_prev_bound_box = m_bound_box;
					m_last_bound_box_change_time = static_cast<platform::bc_clock::small_time>(p_context.m_clock.m_total_elapsed) + 1.f; // +1ms to avoid floating point precision problem

					if (m_added_to_scene)
					{
						m_scene->update_actor(p_context.m_actor);
					}
				}
				m_bound_box = l_bound_box_event->get_bound_box();
				return;
			}

			if (const auto* l_added_to_scene_event = core::bci_message::as<bc_added_to_scene_actor_event>(p_context.m_event))
			{
				m_added_to_scene = true;
				m_scene = &l_added_to_scene_event->get_scene();
				m_prev_bound_box = m_bound_box; // Reset default prev bb to be as same as current bb when actor is added to scene graph
				return;
			}

			if (const auto* l_remove_from_scene_event = core::bci_message::as<bc_removed_from_scene_actor_event>(p_context.m_event))
			{
				m_added_to_scene = false;
				m_scene = nullptr;
				return;
			}
		}
		
		void bc_mediate_component::debug_draw(const bc_actor_component_debug_draw_context& p_context)
		{
			p_context.m_shape_drawer.draw_wired_bound_box(m_bound_box);
		}
	}
}