// [05/27/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"

namespace black_cat
{
	namespace game
	{
		bc_network_component::bc_network_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept
			: bci_actor_component(p_actor_index, p_index),
			m_id(bc_actor::invalid_id)
		{
		}

		bc_network_component::bc_network_component(bc_network_component&&) noexcept = default;

		bc_network_component::~bc_network_component() = default;

		bc_network_component& bc_network_component::operator=(bc_network_component&&) noexcept = default;

		bc_actor bc_network_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_network_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
		}

		void bc_network_component::initialize_entity(const bc_actor_component_initialize_entity_context& p_context)
		{
		}

		void bc_network_component::handle_event(const bc_actor_component_event_context& p_context)
		{
		}
	}	
}