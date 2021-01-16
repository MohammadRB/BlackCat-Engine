// [01/19/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_actor_component_manager& bci_actor_component::get_manager() noexcept
		{
			static bc_actor_component_manager* s_manager = core::bc_get_service<bc_actor_component_manager>();
			return *s_manager;
		}
	}
}