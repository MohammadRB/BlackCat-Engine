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
		
		class bc_iactor_controller
		{
		public:
			virtual ~bc_iactor_controller() = 0;

			virtual void initialize(bc_actor& p_actor);

			virtual void added_to_scene(bc_actor& p_actor, bc_scene& p_scene);
			
			virtual void handle_event(bc_actor& p_actor, const bc_actor_event& p_event);
			
			virtual void update(bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock);
		};

		inline bc_iactor_controller::~bc_iactor_controller() = default;

		inline void bc_iactor_controller::initialize(bc_actor& p_actor)
		{
		}

		inline void bc_iactor_controller::added_to_scene(bc_actor& p_actor, bc_scene& p_scene)
		{
		}

		inline void bc_iactor_controller::handle_event(bc_actor& p_actor, const bc_actor_event& p_event)
		{
		}

		inline void bc_iactor_controller::update(bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock)
		{
		}
	}
}