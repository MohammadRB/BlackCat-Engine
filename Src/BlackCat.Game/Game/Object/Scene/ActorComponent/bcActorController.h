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
		
		class bci_actor_controller
		{
		public:
			virtual ~bci_actor_controller() = 0;

			virtual void initialize(bc_actor& p_actor);

			virtual void added_to_scene(bc_actor& p_actor, bc_scene& p_scene);
						
			virtual void update(bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock);

			virtual void handle_event(bc_actor& p_actor, const bc_actor_event& p_event);
		};

		inline bci_actor_controller::~bci_actor_controller() = default;

		inline void bci_actor_controller::initialize(bc_actor& p_actor)
		{
		}

		inline void bci_actor_controller::added_to_scene(bc_actor& p_actor, bc_scene& p_scene)
		{
		}

		inline void bci_actor_controller::update(bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock)
		{
		}
		
		inline void bci_actor_controller::handle_event(bc_actor& p_actor, const bc_actor_event& p_event)
		{
		}
	}
}