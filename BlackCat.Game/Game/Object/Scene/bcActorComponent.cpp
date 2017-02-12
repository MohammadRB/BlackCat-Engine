// [01/19/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/bcActorComponentManager.h"
#include "Game/Object/Scene/bcActorComponent.h"

namespace black_cat
{
	namespace game
	{
		inline bc_actor_component_manager* bc_iactor_component::_get_manager() noexcept
		{
			static bc_actor_component_manager* s_manager = core::bc_get_service<bc_actor_component_manager>();

			return s_manager;
		}
	}
}