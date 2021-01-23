// [01/23/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcMeshUtility.h"
#include "Game/Object/Mesh/bcSubMesh.h"

namespace black_cat
{
	namespace game
	{
		void bc_mesh_utility::calculate_bound_box(const bc_mesh& p_mesh, const bc_sub_mesh_transform& p_transforms, physics::bc_bound_box& p_bound_box) noexcept
		{
			p_bound_box.set_empty();

			int l_dummy = 0;
			iterate_over_nodes(p_mesh, l_dummy, [&p_mesh, &p_transforms, &p_bound_box](const bc_mesh_node& p_node, int)
			{
				const auto& l_node_transform = p_transforms.get_node_transform(p_node);

				// Update mesh bounding box based on its sub meshes
				for (bcSIZE l_mesh_ite = 0; l_mesh_ite < p_node.get_mesh_count(); ++l_mesh_ite)
				{
					auto l_node_mesh_box = p_mesh.get_node_mesh_bound_box(p_node, l_mesh_ite);
					l_node_mesh_box.transform(physics::bc_transform(l_node_transform));

					if (p_bound_box.is_empty())
					{
						p_bound_box = l_node_mesh_box;
					}
					else
					{
						p_bound_box.expand(l_node_mesh_box);
					}
				}

				return 0;
			});
		}

		void bc_mesh_utility::calculate_absolute_transforms(const bc_mesh& p_mesh, const core::bc_matrix4f& p_world, bc_sub_mesh_transform& p_transforms, physics::bc_bound_box& p_bound_box) noexcept
		{
			p_bound_box.set_empty();

			iterate_over_nodes(p_mesh, p_world, [&p_mesh, &p_transforms, &p_bound_box](const bc_mesh_node& p_node, const core::bc_matrix4f& p_parent_transform)
			{
				const auto l_node_absolute_transformation = p_mesh.get_node_transform(p_node) * p_parent_transform;
				p_transforms.set_node_transform
				(
					p_node,
					l_node_absolute_transformation
				);

				// Update mesh bounding box based on its sub meshes
				for (bcSIZE l_m = 0; l_m < p_node.get_mesh_count(); ++l_m)
				{
					auto l_node_mesh_box = p_mesh.get_node_mesh_bound_box(p_node, l_m);
					l_node_mesh_box.transform(physics::bc_transform(l_node_absolute_transformation));

					if (p_bound_box.is_empty())
					{
						p_bound_box = l_node_mesh_box;
					}
					else
					{
						p_bound_box.expand(l_node_mesh_box);
					}
				}

				return l_node_absolute_transformation;
			});
		}

		void bc_mesh_utility::calculate_skinned_mesh_colliders_inverse_bind_pose(const bc_mesh& p_mesh, const bc_sub_mesh_transform& p_model_transforms, bc_sub_mesh_transform& p_transforms)
		{
			for (const auto& l_mesh_part_collider : p_mesh.get_collider())
			{
				for (const bc_mesh_part_collider_entry& l_entry : l_mesh_part_collider.second)
				{
					const auto* l_collider_attached_node = p_mesh.find_node(l_entry.m_attached_node_transform_index);
					BC_ASSERT(l_collider_attached_node);
					
					const auto& l_scaled_inverse_bind_pose = p_mesh.get_node_scaled_inverse_bind_pose_transform(*l_collider_attached_node);
					p_transforms[l_entry.m_attached_node_transform_index] = p_model_transforms[l_entry.m_attached_node_transform_index] * l_scaled_inverse_bind_pose;
				}
			}
		}
	}
}