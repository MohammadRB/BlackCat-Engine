// [16/12/2020 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene;
		class bc_shape_drawer;
		class bc_controller_component;
		
		class bci_actor_controller
		{
			friend class bc_controller_component;

		public:
			virtual ~bci_actor_controller() = 0;

		protected:
			bc_scene* get_scene() noexcept;
					
			virtual void initialize(const bc_actor_component_initialize_context& p_context);

			virtual void load_instance(const bc_actor_component_load_context& p_context);

			virtual void write_instance(const bc_actor_component_write_context& p_context);

			virtual void load_network_instance(const bc_actor_component_network_load_context& p_context);

			virtual void write_network_instance(const bc_actor_component_network_write_context& p_context);

			virtual void added_to_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene);

			virtual void update(const bc_actor_component_update_content& p_context);

			virtual void removed_from_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene);

			virtual void handle_event(const bc_actor_component_event_context& p_context);

			virtual void debug_draw(const bc_actor_component_debug_draw_context& p_context);

		private:
			void _handle_event(const bc_actor_component_event_context& p_context);

			bc_scene* m_scene{ nullptr };
		};

		inline bci_actor_controller::~bci_actor_controller() = default;

		inline void bci_actor_controller::initialize(const bc_actor_component_initialize_context& p_context)
		{
		}

		inline void bci_actor_controller::load_instance(const bc_actor_component_load_context& p_context)
		{
		}

		inline void bci_actor_controller::write_instance(const bc_actor_component_write_context& p_context)
		{
		}

		inline void bci_actor_controller::load_network_instance(const bc_actor_component_network_load_context& p_context)
		{
		}

		inline void bci_actor_controller::write_network_instance(const bc_actor_component_network_write_context& p_context)
		{
		}

		inline void bci_actor_controller::added_to_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene)
		{
		}

		inline void bci_actor_controller::update(const bc_actor_component_update_content& p_context)
		{
		}

		inline void bci_actor_controller::removed_from_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene)
		{
		}
		
		inline void bci_actor_controller::handle_event(const bc_actor_component_event_context& p_context)
		{
		}

		inline void bci_actor_controller::debug_draw(const bc_actor_component_debug_draw_context& p_context)
		{
		}

		inline void bci_actor_controller::_handle_event(const bc_actor_component_event_context& p_context)
		{
			if (const auto* l_added_to_scene_event = core::bci_message::as<bc_added_to_scene_actor_event>(p_context.m_event))
			{
				m_scene = &l_added_to_scene_event->get_scene();
				added_to_scene(p_context, l_added_to_scene_event->get_scene());
				return;
			}

			if (const auto* l_remove_from_scene_event = core::bci_message::as<bc_removed_from_scene_actor_event>(p_context.m_event))
			{
				removed_from_scene(p_context, l_remove_from_scene_event->get_scene());
				m_scene = nullptr;
				return;
			}

			handle_event(p_context);
		}

		inline bc_scene* bci_actor_controller::get_scene() noexcept
		{
			return m_scene;
		}
	}
}