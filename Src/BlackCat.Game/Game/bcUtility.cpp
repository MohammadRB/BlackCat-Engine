// [12/31/2020 MRB]

#include "Game/GamePCH.h"
#include "Game/bcUtility.h"
#include "Game/Object/Mesh/bcMesh.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/System/Render/bcRenderStateBuffer.h"

namespace black_cat
{
	void render_mesh(game::bc_render_state_buffer& p_buffer,
		const game::bc_sub_mesh& p_mesh_part,
		const game::bc_sub_mesh_transform& p_transformations,
		const game::bc_mesh_node* const* p_begin,
		const game::bc_mesh_node* const* p_end,
		const bcCHAR* p_mesh_prefix)
	{
		for (; p_begin != p_end; ++p_begin)
		{
			const game::bc_mesh_node* l_node = *p_begin;

			for (game::bc_mesh_node::node_index_t l_mesh_index = 0, l_mesh_count = l_node->get_mesh_count(); l_mesh_index < l_mesh_count; ++l_mesh_index)
			{
				if (p_mesh_prefix != nullptr)
				{
					const auto& l_mesh_name = p_mesh_part.get_node_mesh_name(*l_node, l_mesh_index);
					const bool l_starts_with_prefix = l_mesh_name.compare(0, std::strlen(p_mesh_prefix), p_mesh_prefix) == 0;

					if (!l_starts_with_prefix)
					{
						continue;
					}
				}

				auto l_node_mesh_render_state = p_mesh_part.get_node_mesh_render_state_ptr(*l_node, l_mesh_index);
				const auto& l_node_transformation = p_mesh_part.get_node_absolute_transform(*l_node, p_transformations);

				game::bc_render_instance l_instance(l_node_transformation);
				p_buffer.add_render_instance(std::move(l_node_mesh_render_state), l_instance);
			}

			const auto& l_node_children = p_mesh_part.get_node_children(*l_node);
			if (!l_node_children.empty())
			{
				const game::bc_mesh_node* const* l_begin = &l_node_children.front();
				const game::bc_mesh_node* const* l_end = &l_node_children.back() + 1;

				render_mesh
				(
					p_buffer,
					p_mesh_part,
					p_transformations,
					l_begin,
					l_end,
					p_mesh_prefix
				);
			}
		}
	}

	void render_skinned_mesh(game::bc_render_state_buffer& p_buffer,
		const game::bc_sub_mesh& p_mesh_part,
		const game::bc_sub_mesh_transform& p_transformations,
		const game::bc_mesh_node* const* p_begin,
		const game::bc_mesh_node* const* p_end)
	{
		for (; p_begin != p_end; ++p_begin)
		{
			const game::bc_mesh_node* l_node = *p_begin;

			for (game::bc_mesh_node::node_index_t l_mesh_index = 0, l_mesh_count = l_node->get_mesh_count(); l_mesh_index < l_mesh_count; ++l_mesh_index)
			{
				auto l_node_mesh_render_state = p_mesh_part.get_node_mesh_render_state_ptr(*l_node, l_mesh_index);
				
				game::bc_skinned_render_instance l_instance(p_transformations.size());
				p_transformations.copy_transformations_to(l_instance.get_transforms());
				
				p_buffer.add_skinned_render_instance(std::move(l_node_mesh_render_state), std::move(l_instance));
			}

			const auto& l_node_children = p_mesh_part.get_node_children(*l_node);
			if (!l_node_children.empty())
			{
				const game::bc_mesh_node* const* l_begin = &l_node_children.front();
				const game::bc_mesh_node* const* l_end = &l_node_children.back() + 1;

				render_skinned_mesh
				(
					p_buffer,
					p_mesh_part,
					p_transformations,
					l_begin,
					l_end
				);
			}
		}
	}
}