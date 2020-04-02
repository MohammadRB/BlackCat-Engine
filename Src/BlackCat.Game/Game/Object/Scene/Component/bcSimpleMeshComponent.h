// [09/09/2019 MRB]

#pragma once

#include "Game/bcExport.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_simple_mesh_component : public bc_mesh_component
		{
			BC_COMPONENT(smp_msh, true, false)

		public:
			explicit bc_simple_mesh_component(bc_actor_component_index p_index);

			bc_simple_mesh_component(bc_simple_mesh_component&& p_other) noexcept;

			~bc_simple_mesh_component();

			bc_simple_mesh_component& operator=(bc_simple_mesh_component&& p_other) noexcept;

			bc_actor get_actor() const noexcept override;

			void handle_event(bc_actor& p_actor, const bc_actor_event& p_event) override;

			void update(bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param) override;
		};
	}
}