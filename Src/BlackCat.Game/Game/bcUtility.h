// [12/31/2020 MRB]

#pragma once

#include "Core/bcUtility.h"
#include "Game/Object/Mesh/bcSubMeshTransform.h"

namespace black_cat
{
	namespace game
	{
		class bc_mesh_node;
		class bc_sub_mesh;
		class bc_render_state_buffer;
	}

	void render_mesh(game::bc_render_state_buffer& p_buffer,
		const game::bc_sub_mesh& p_mesh_part,
		const game::bc_sub_mesh_mat4_transform& p_transformations,
		const game::bc_mesh_node* const* p_begin,
		const game::bc_mesh_node* const* p_end,
		const bcCHAR* p_mesh_prefix = nullptr);

	void render_skinned_mesh(game::bc_render_state_buffer& p_buffer,
		const game::bc_sub_mesh& p_mesh_part,
		const game::bc_sub_mesh_mat4_transform& p_transformations,
		const game::bc_mesh_node* const* p_begin,
		const game::bc_mesh_node* const* p_end);
}