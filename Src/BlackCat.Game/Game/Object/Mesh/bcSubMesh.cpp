// [02/09/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Physics/bcPhysicsShapeUtility.h"
#include "Game/Object/Mesh/bcSubMesh.h"

namespace black_cat
{
	namespace game
	{
		bc_sub_mesh::bc_sub_mesh()
			: m_mesh(nullptr),
			m_node(nullptr)
		{
		}

		bc_sub_mesh::bc_sub_mesh(bc_mesh_ptr p_mesh)
		{
			if (p_mesh == nullptr)
			{
				throw bc_invalid_argument_exception("null argument");
			}

			m_mesh = p_mesh;
			m_node = p_mesh->get_root();
		}

		bc_sub_mesh::bc_sub_mesh(bc_mesh_ptr p_mesh, const bcCHAR* p_node)
		{
			if (p_mesh == nullptr)
			{
				throw bc_invalid_argument_exception("null argument");
			}

			m_mesh = p_mesh;
			m_node = p_mesh->find_node(p_node);

			if (m_node == nullptr)
			{
				throw bc_invalid_argument_exception("null argument");
			}
		}

		bc_sub_mesh::bc_sub_mesh(bc_sub_mesh&& p_other) noexcept
			: m_mesh(std::move(p_other.m_mesh)),
			m_node(p_other.m_node)
		{
		}

		bc_sub_mesh::~bc_sub_mesh()
		{
		}

		bc_sub_mesh& bc_sub_mesh::operator=(bc_sub_mesh&& p_other) noexcept
		{
			m_mesh = std::move(p_other.m_mesh);
			m_node = p_other.m_node;

			return *this;
		}

		const core::bc_matrix4f* bc_sub_mesh::get_node_absolute_transformation(const bc_mesh_node* p_node, const bc_sub_mesh_transformation& p_transformations) const
		{
			auto l_root_node_index = p_transformations.m_root_node_index;
			auto l_node_index = p_node->get_transformation_index();

			bcAssert(l_root_node_index != bc_mesh_node::node_indexing(-1) && l_node_index >= l_root_node_index);

			return &p_transformations.m_transformations[l_node_index - l_root_node_index];
		}

		void bc_sub_mesh::calculate_absolute_transformations(const core::bc_matrix4f& p_world, bc_sub_mesh_transformation& p_result, physics::bc_bound_box& p_bound_box) const
		{
			const bc_mesh_node* l_begin = m_node;
			p_result.m_root_node_index = l_begin->get_transformation_index();
			p_result.m_transformations.clear();
			p_bound_box.set_empty();

			_calculate_absolute_transformations(p_world, l_begin, l_begin + 1, p_result, p_bound_box);
		}

		void bc_sub_mesh::_calculate_absolute_transformations(const core::bc_matrix4f& p_parent_transformation,
			const bc_mesh_node* p_begin,
			const bc_mesh_node* p_end,
			bc_sub_mesh_transformation& p_result,
			physics::bc_bound_box& p_bound_box) const
		{
			for (; p_begin != p_end; ++p_begin)
			{
				const bc_mesh_node* l_node = p_begin;

				auto l_node_absolute_transformation = *m_mesh->get_node_transformation(l_node) * p_parent_transformation;
				p_result.m_transformations.push_back(l_node_absolute_transformation);

				// Update mesh bounding box based on its sub meshes
				for (bcSIZE m = 0; m < l_node->get_mesh_count(); ++m)
				{
					auto l_node_mesh_box = *m_mesh->get_node_mesh_bound_box(l_node, m);
					l_node_mesh_box.transform(l_node_absolute_transformation);

					if (p_bound_box.is_empty())
					{
						p_bound_box = l_node_mesh_box;
					}
					else
					{
						p_bound_box.expand(l_node_mesh_box);
					}
				}

				const auto& l_node_children = m_mesh->get_node_children(l_node);
				if (!l_node_children.empty())
				{
					const bc_mesh_node* l_begin = *l_node_children.begin();
					const bc_mesh_node* l_end = l_begin + l_node_children.size();

					_calculate_absolute_transformations
					(
						l_node_absolute_transformation,
						l_begin,
						l_end,
						p_result,
						p_bound_box
					);
				}
			}
		}
	}
}