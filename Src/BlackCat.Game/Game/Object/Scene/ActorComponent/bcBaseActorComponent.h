// [10/12/2023 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class BC_GAME_DLL bc_base_actor_component : public bci_actor_component
	{
	public:
		~bc_base_actor_component() override = default;

	protected:
		bc_base_actor_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept;

		bc_base_actor_component(bc_base_actor_component&&) noexcept = default;

		bc_base_actor_component& operator=(bc_base_actor_component&&) noexcept = default;

		const core::bc_matrix4f& get_transform() const noexcept;

		core::bc_vector3f get_translation() const noexcept;
	};
}
