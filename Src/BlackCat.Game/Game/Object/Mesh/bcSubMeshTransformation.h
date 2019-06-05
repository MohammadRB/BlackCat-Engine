// [06/07/2016 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Math/bcMatrix4f.h"
#include "Game/Object/Mesh/bcMesh.h"

namespace black_cat
{
	namespace game
	{
		class bc_sub_mesh_transformation
		{
		public:
			bc_sub_mesh_transformation()
				: m_root_node_index(bc_mesh_node::s_invalid_index)
			{
			}

			explicit bc_sub_mesh_transformation(const bc_mesh_node& p_root_node)
				: m_root_node_index(p_root_node.get_transformation_index()),
				m_transformations(p_root_node.get_all_children_count() + 1, core::bc_matrix4f::identity()) // + 1 for root node transformation
			{
			}

			bc_sub_mesh_transformation(const bc_sub_mesh_transformation&) = default;

			bc_sub_mesh_transformation(bc_sub_mesh_transformation&&) = default;

			~bc_sub_mesh_transformation() = default;

			bc_sub_mesh_transformation& operator=(const bc_sub_mesh_transformation&) = default;

			bc_sub_mesh_transformation& operator=(bc_sub_mesh_transformation&&) = default;

			bc_mesh_node::node_index get_root_node_index() const
			{
				return m_root_node_index;
			}

			core::bc_vector3f get_node_translation(const bc_mesh_node& p_node) const
			{
				return m_transformations[p_node.get_transformation_index() - m_root_node_index].get_translation();
			}

			const core::bc_matrix4f& get_node_transform(const bc_mesh_node& p_node) const
			{
				bcAssert(m_root_node_index != bc_mesh_node::s_invalid_index && p_node.get_transformation_index() >= m_root_node_index);

				return m_transformations[p_node.get_transformation_index() - m_root_node_index];
			}

			void set_node_transformation(const bc_mesh_node& p_node, const core::bc_matrix4f& p_transform)
			{
				m_transformations.at(p_node.get_transformation_index() - m_root_node_index) = p_transform;
			}

		protected:

		private:
			bc_mesh_node::node_index m_root_node_index;
			core::bc_vector_movale<core::bc_matrix4f> m_transformations;
		};
	}
}