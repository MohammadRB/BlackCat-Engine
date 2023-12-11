// [09/09/2019 MRB]

#pragma once

#include "Game/System/Render/bcRenderInstance.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class BC_GAME_DLL bc_simple_mesh_component : public bc_mesh_component
	{
		BC_COMPONENT(smp_msh, true, false)

	public:
		bc_simple_mesh_component(bc_actor_id p_actor_id, bc_actor_component_id p_id);

		bc_simple_mesh_component(bc_simple_mesh_component&& p_other) noexcept;

		~bc_simple_mesh_component() override;

		bc_simple_mesh_component& operator=(bc_simple_mesh_component&& p_other) noexcept;
		
		void initialize_entity(const bc_actor_component_initialize_entity_context& p_context) override;
			
		void render(const bc_actor_component_render_context& p_context) const override;
			
		void handle_event(const bc_actor_component_event_context& p_context) override;

		void add_decal(const bcCHAR* p_decal_name,
		               const core::bc_vector3f& p_world_position,
		               const core::bc_vector3f& p_world_direction,
		               bc_mesh_node::node_index_t p_attached_node_index) override;
			
	private:
		bc_actor_render_group m_render_group;
	};
}
