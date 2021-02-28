// [09/09/2019 MRB]

#pragma once

#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_simple_mesh_component : public bc_mesh_component
		{
			BC_COMPONENT(smp_msh, true, false)

		public:
			bc_simple_mesh_component(bc_actor_index p_actor_index, bc_actor_component_index p_index);

			bc_simple_mesh_component(bc_simple_mesh_component&& p_other) noexcept;

			~bc_simple_mesh_component() override;

			bc_simple_mesh_component& operator=(bc_simple_mesh_component&& p_other) noexcept;

			bc_actor get_actor() const noexcept override;

			void handle_event(const bc_actor_component_event_context& p_context) override;
		};
	}
}