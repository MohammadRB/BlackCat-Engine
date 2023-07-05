// [19/01/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/bcEntityManager.h"

namespace black_cat::game
{
	bc_actor_component_manager_container& bci_actor_component::get_manager() const noexcept
	{
		static auto& s_manager = *core::bc_get_service<bc_entity_manager>();
		return s_manager.get_actor_component_container(bc_actor(m_actor_id));
	}
}
