// [10/12/2023 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorComponent/bcBaseActorComponent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"

namespace black_cat::game
{
	_bc_base_actor_component::_bc_base_actor_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept
		: bci_actor_component(p_actor_id, p_id)
	{
	}

	const core::bc_matrix4f& _bc_base_actor_component::get_transform() const noexcept
	{
		const auto* l_mediate_component = get_actor().get_component<bc_mediate_component>();
		return l_mediate_component->get_world_transform();
	}

	core::bc_vector3f _bc_base_actor_component::get_translation() const noexcept
	{
		const auto* l_mediate_component = get_actor().get_component<bc_mediate_component>();
		return l_mediate_component->get_position();
	}
}
