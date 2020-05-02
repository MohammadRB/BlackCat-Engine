// [09/06/2019 MRB]

#pragma once

#include "Game/bcExport.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_vegetable_mesh_component : public bc_mesh_component
		{
			BC_COMPONENT(veg_msh, true, false)

		public:
			explicit bc_vegetable_mesh_component(bc_actor_component_index p_index);

			bc_vegetable_mesh_component(bc_vegetable_mesh_component&& p_other) noexcept;

			~bc_vegetable_mesh_component();

			bc_vegetable_mesh_component& operator=(bc_vegetable_mesh_component&&) noexcept;

			bc_actor get_actor() const noexcept override;

			void initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) override;

			void handle_event(bc_actor& p_actor, const bc_actor_event& p_event) override;

			void render(bc_render_state_buffer& p_buffer) const override;

			void render(bc_render_state_buffer& p_buffer, bool p_render_leaf) const;
		};
	}
}