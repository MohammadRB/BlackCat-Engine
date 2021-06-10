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
			bc_vegetable_mesh_component(bc_actor_id p_actor_index, bc_actor_component_id p_index);

			bc_vegetable_mesh_component(bc_vegetable_mesh_component&& p_other) noexcept;

			~bc_vegetable_mesh_component() override;

			bc_vegetable_mesh_component& operator=(bc_vegetable_mesh_component&&) noexcept;

			bc_actor get_actor() const noexcept override;

			void initialize(const bc_actor_component_initialize_context& p_context) override;

			void handle_event(const bc_actor_component_event_context& p_context) override;

			void render(const bc_actor_component_render_context& p_context) const override;

			void render(const bc_actor_component_render_context& p_context, bool p_render_leaf) const;

			void add_decal(const bcCHAR* p_decal_name,
				const core::bc_vector3f& p_world_position,
				const core::bc_vector3f& p_world_direction,
				bc_mesh_node::node_index_t p_attached_node_index) override;
			
		private:
			bc_mesh_render_state m_leaf_render_state;
			bc_mesh_render_state m_trunk_render_state;
		};
	}
}