// [12/22/2018 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcJsonParse.h"
#include "Core/Utility/bcLogger.h"
#include "Game/System/Render/bcShapeDrawer.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"
#include "Game/bcUtility.h"

namespace black_cat
{
	namespace game
	{
		bc_mediate_component::bc_mediate_component(bc_actor_id p_actor_index, bc_actor_component_id p_index)
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
		}

		void bc_mediate_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			m_scene = &p_context.m_scene;
		}

		void bc_mediate_component::load_instance(const bc_actor_component_load_context& p_context)
		{
			if (m_controller)
			{
				m_controller->load_instance(p_context);
			}
		}

		void bc_mediate_component::write_instance(const bc_actor_component_write_context& p_context)
		{
			if (m_controller)
			{
				m_controller->write_instance(p_context);
			}
		}

		void bc_mediate_component::load_network_instance(const bc_actor_component_network_load_context& p_context)
		{
			if (m_controller)
			{
				m_controller->load_network_instance(p_context);
			}
		}

		void bc_mediate_component::write_network_instance(const bc_actor_component_network_write_context& p_context)
		{
			if (m_controller)
			{
				m_controller->write_network_instance(p_context);
			}
		}

		void bc_mediate_component::update(const bc_actor_component_update_content& p_context)
		{
			if(m_scene && m_bound_box_changed && !p_context.m_is_double_update)
			{
				m_scene->update_actor(p_context.m_actor);
				m_bound_box_changed = false;
			}

			if(m_controller)
			{
				m_controller->update(p_context);
			}
		}

		void bc_mediate_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			_handle_event(p_context);

			if (m_controller)
			{
				m_controller->handle_event(p_context);
			}
		}
		
		void bc_mediate_component::debug_draw(const bc_actor_component_debug_draw_context& p_context)
		{
			p_context.m_shape_drawer.draw_wired_bound_box(m_bound_box);

			if(m_controller)
			{
				m_controller->debug_draw(p_context);
			}
		}

		void bc_mediate_component::_handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_transformation_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
			if (l_transformation_event)
			{
				m_world_transform = l_transformation_event->get_transform();
				return;
			}

			const auto* l_bound_box_event = core::bci_message::as<bc_bound_box_changed_actor_event>(p_context.m_event);
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

			const auto* l_added_to_scene_event = core::bci_message::as<bc_added_to_scene_actor_event>(p_context.m_event);
			if (l_added_to_scene_event)
			{
				m_scene = &l_added_to_scene_event->get_scene();
				if (m_controller)
				{
					m_controller->added_to_scene(p_context, *m_scene);
				}
				return;
			}

			const auto* l_remove_from_scene_event = core::bci_message::as<bc_removed_from_scene_actor_event>(p_context.m_event);
			if (l_remove_from_scene_event)
			{
				if (m_controller)
				{
					m_controller->removed_from_scene(p_context, l_remove_from_scene_event->get_scene());
					m_scene = nullptr;
				}
				return;
			}
		}
	}
}