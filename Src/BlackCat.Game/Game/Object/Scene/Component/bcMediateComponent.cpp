// [12/22/2018 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/bcShapeDrawer.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/Event/bcActorEventWorldTransform.h"
#include "Game/Object/Scene/Component/Event/bcActorEventBoundBoxChanged.h"
#include "Game/Object/Scene/Component/Event/bcActorEventAddedToScene.h"

namespace black_cat
{
	namespace game
	{
		bc_mediate_component::bc_mediate_component(bc_actor_index p_actor_index, bc_actor_component_index p_index)
			: bci_actor_component(p_actor_index, p_index),
			m_entity_name(nullptr),
			m_scene(nullptr),
			m_bound_box_changed(false),
			m_prev_bound_box(),
			m_bound_box(),
			m_controller(nullptr)
		{
		}

		bc_actor bc_mediate_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_mediate_component::set_controller(core::bc_unique_ptr<bci_actor_controller> p_controller)
		{
			m_controller = std::move(p_controller);

			auto l_actor = get_actor();
			m_controller->initialize(l_actor);
		}

		void bc_mediate_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
		}
				
		void bc_mediate_component::update(const bc_actor_component_update_content& p_context)
		{
			if(m_scene && m_bound_box_changed)
			{
				m_scene->update_actor(p_context.m_actor);
				m_bound_box_changed = false;
			}

			if(m_controller)
			{
				m_controller->update(p_context.m_actor, p_context.m_clock);
			}
		}

		void bc_mediate_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			_handle_event(p_context.m_actor, p_context.m_event);

			if (m_controller)
			{
				m_controller->handle_event(p_context.m_actor, p_context.m_event);
			}
		}
		
		void bc_mediate_component::debug_draw(const bc_actor_component_debug_draw_context& p_context)
		{
			p_context.m_shape_drawer.draw_wired_bound_box(m_bound_box);

			if(m_controller)
			{
				m_controller->debug_draw(p_context.m_actor, p_context.m_shape_drawer);
			}
		}

		void bc_mediate_component::_handle_event(bc_actor& p_actor, const bc_actor_event& p_event)
		{
			const auto* l_transformation_event = core::bci_message::as< bc_actor_event_world_transform >(p_event);
			if (l_transformation_event)
			{
				m_world_transform = l_transformation_event->get_transform();
				return;
			}

			const auto* l_bound_box_event = core::bci_message::as< bc_actor_event_bound_box_changed >(p_event);
			if (l_bound_box_event)
			{
				if(!m_bound_box_changed) // update prev box once only in case of multiple events per frame
				{
					m_prev_bound_box = m_bound_box;
				}
				m_bound_box = l_bound_box_event->get_bound_box();
				m_bound_box_changed = true;
				return;
			}

			const auto* l_added_to_scene_event = core::bci_message::as< bc_actor_event_added_to_scene >(p_event);
			if (l_added_to_scene_event)
			{
				m_scene = &l_added_to_scene_event->get_scene();
				if (m_controller)
				{
					m_controller->added_to_scene(p_actor, *m_scene);
				}
				return;
			}
		}
	}
}