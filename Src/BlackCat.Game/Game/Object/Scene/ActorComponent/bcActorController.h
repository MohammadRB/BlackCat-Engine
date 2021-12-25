// [12/16/2020 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene;
		class bc_shape_drawer;
		class bc_mediate_component;
		struct bc_actor_component_initialize_context;
		struct bc_actor_component_load_context;
		struct bc_actor_component_write_context;
		struct bc_actor_component_network_load_context;
		struct bc_actor_component_network_write_context;
		struct bc_actor_component_update_content;
		struct bc_actor_component_event_context;
		struct bc_actor_component_debug_draw_context;
		
		class bci_actor_controller
		{
			friend class bc_mediate_component;

		public:
			virtual ~bci_actor_controller() = 0;

		protected:
			bc_scene* get_scene() const noexcept;
		
		private:
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
			m_scene = &p_scene;
		}

		inline void bci_actor_controller::update(const bc_actor_component_update_content& p_context)
		{
		}

		inline void bci_actor_controller::removed_from_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene)
		{
			m_scene = nullptr;
		}
		
		inline void bci_actor_controller::handle_event(const bc_actor_component_event_context& p_context)
		{
		}

		inline void bci_actor_controller::debug_draw(const bc_actor_component_debug_draw_context& p_context)
		{
		}

		inline bc_scene* bci_actor_controller::get_scene() const noexcept
		{
			return m_scene;
		}
	}
}