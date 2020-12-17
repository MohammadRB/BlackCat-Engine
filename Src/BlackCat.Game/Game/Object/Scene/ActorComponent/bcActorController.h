// [12/16/2020 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"

namespace black_cat
{
	namespace game
	{
		class bc_iactor_controller
		{
		public:
			virtual ~bc_iactor_controller() = default;

			virtual void initialize(bc_actor& p_actor) = 0;

			virtual void handle_event(bc_actor& p_actor, const bc_actor_event& p_event) = 0;
			
			virtual void update(bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock) = 0;
		};
	}
}