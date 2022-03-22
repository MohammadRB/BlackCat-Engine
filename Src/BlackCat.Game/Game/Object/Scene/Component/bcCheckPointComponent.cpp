// [01/27/2022 MRB]

#include "Game/GamePCH.h"

#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcCheckPointComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_checkpoint_component::bc_checkpoint_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept
			: bci_actor_component(p_actor_id, p_id)
		{
		}

		bc_actor bc_checkpoint_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}
	}
}
