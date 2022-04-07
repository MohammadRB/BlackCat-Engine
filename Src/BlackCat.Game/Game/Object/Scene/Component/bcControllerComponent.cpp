// [04/07/2022 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcControllerComponent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"

namespace black_cat
{
	namespace game
	{
		bc_controller_component::bc_controller_component(bc_actor_id p_actor_id, bc_actor_component_id p_id)
			: bci_actor_component(p_actor_id, p_id)
		{
		}

		void bc_controller_component::set_controller(core::bc_unique_ptr<bci_actor_controller> p_controller, const bc_actor_component_initialize_context& p_context) noexcept
		{
			m_controller = std::move(p_controller);
			m_controller->initialize(p_context);
		}

		bc_actor bc_controller_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_controller_component::load_instance(const bc_actor_component_load_context& p_context)
		{
			m_controller->load_instance(p_context);
		}

		void bc_controller_component::write_instance(const bc_actor_component_write_context& p_context)
		{
			m_controller->write_instance(p_context);
		}

		void bc_controller_component::load_network_instance(const bc_actor_component_network_load_context& p_context)
		{
			m_controller->load_network_instance(p_context);
		}

		void bc_controller_component::write_network_instance(const bc_actor_component_network_write_context& p_context)
		{
			m_controller->write_network_instance(p_context);
		}

		void bc_controller_component::update(const bc_actor_component_update_content& p_context)
		{
			m_controller->update(p_context);
		}

		void bc_controller_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			if (const auto* l_added_to_scene_event = core::bci_message::as<bc_added_to_scene_actor_event>(p_context.m_event))
			{
				m_controller->added_to_scene(p_context, l_added_to_scene_event->get_scene());
				return;
			}

			if (const auto* l_remove_from_scene_event = core::bci_message::as<bc_removed_from_scene_actor_event>(p_context.m_event))
			{
				m_controller->removed_from_scene(p_context, l_remove_from_scene_event->get_scene());
				return;
			}

			m_controller->handle_event(p_context);
		}

		void bc_controller_component::debug_draw(const bc_actor_component_debug_draw_context& p_context)
		{
			m_controller->debug_draw(p_context);
		}
	}
}
