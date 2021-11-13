// [11/13/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcRagdollComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_ragdoll_component::bc_ragdoll_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept
			: bci_actor_component(p_actor_index, p_index)
		{
		}

		bc_ragdoll_component::bc_ragdoll_component(bc_ragdoll_component&&) noexcept = default;

		bc_ragdoll_component::~bc_ragdoll_component() = default;

		bc_ragdoll_component& bc_ragdoll_component::operator=(bc_ragdoll_component&&) noexcept = default;

		bc_actor bc_ragdoll_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_ragdoll_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			auto* l_mesh_component = p_context.m_actor.get_component<bc_mesh_component>();
			auto& l_mesh = l_mesh_component->get_mesh();
			auto& l_mesh_colliders = l_mesh.get_mesh_collider();
		}

		void bc_ragdoll_component::initialize_entity(const bc_actor_component_initialize_entity_context& p_context)
		{
		}

		void bc_ragdoll_component::handle_event(const bc_actor_component_event_context& p_context)
		{
		}
	}	
}