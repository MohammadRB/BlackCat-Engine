// [12/16/2020 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene;
		class bc_shape_drawer;
		struct bc_actor_component_load_context;
		struct bc_actor_component_write_context;
		struct bc_actor_component_update_content;
		struct bc_actor_component_event_context;
		struct bc_actor_component_debug_draw_context;
		
		class bci_actor_controller
		{
		public:
			virtual ~bci_actor_controller() = 0;

			virtual void initialize(bc_actor& p_actor);

			virtual void added_to_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene);
			
			virtual void update(const bc_actor_component_update_content& p_context);

			virtual void debug_draw(const bc_actor_component_debug_draw_context& p_context);
			
			virtual void handle_event(const bc_actor_component_event_context& p_context);
		};

		inline bci_actor_controller::~bci_actor_controller() = default;

		inline void bci_actor_controller::initialize(bc_actor& p_actor)
		{
		}

		inline void bci_actor_controller::added_to_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene)
		{
		}

		inline void bci_actor_controller::update(const bc_actor_component_update_content& p_context)
		{
		}

		inline void bci_actor_controller::debug_draw(const bc_actor_component_debug_draw_context& p_context)
		{
		}

		inline void bci_actor_controller::handle_event(const bc_actor_component_event_context& p_context)
		{
		}
	}
}