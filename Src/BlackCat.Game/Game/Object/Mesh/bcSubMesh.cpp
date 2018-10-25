// [02/09/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcSubMesh.h"

namespace black_cat
{
	namespace game
	{
		void bc_sub_mesh::calculate_absolute_transformations(const core::bc_matrix4f& p_world, bc_mesh_part_transformation& p_result) const
		{
			const bc_mesh_node* l_begin = m_node;
			p_result.m_root_node_index = m_node->get_transformation_index();
			p_result.m_transformations.clear();

			_calculate_absolute_transformations(p_world, l_begin, l_begin + 1, p_result);
		}

		void bc_sub_mesh::_calculate_absolute_transformations(const core::bc_matrix4f& p_parent_transformation,
			const bc_mesh_node* p_begin,
			const bc_mesh_node* p_end,
			bc_mesh_part_transformation& p_result) const
		{
			for (; p_begin != p_end; ++p_begin)
			{
				const bc_mesh_node* l_node = p_begin;

				auto l_transformation = *m_mesh->get_node_transformation(l_node) * p_parent_transformation;
				p_result.m_transformations.push_back(l_transformation);

				const auto& l_node_childs = m_mesh->get_node_childs(l_node);

				if (l_node_childs.size() > 0)
				{
					const bc_mesh_node* l_begin = *l_node_childs.begin();
					const bc_mesh_node* l_end = l_begin + l_node_childs.size();

					_calculate_absolute_transformations
					(
						l_transformation,
						l_begin,
						l_end,
						p_result
					);
				}
			}
		}
	}
}
