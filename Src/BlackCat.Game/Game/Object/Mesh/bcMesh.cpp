// [06/14/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/bcException.h"
#include "Game/Object/Mesh/bcMesh.h"
#include "Game/Object/Mesh/bcMeshPartCollider.h"
#include "Game/Object/Mesh/bcSubMeshTransform.h"

namespace black_cat
{
	namespace game
	{
		bc_mesh::bc_mesh(bc_mesh_builder p_builder, bc_mesh_collider_ptr p_colliders)
			: m_name(std::move(p_builder.m_name)),
			m_root(nullptr),
			m_scale(1),
			m_colliders(std::move(p_colliders))
		{
			// Reserve needed memory for nodes because we use raw pointers in bc_mesh_node for parent and children
			m_nodes.resize(p_builder.m_node_count);
			m_nodes_map.reserve(p_builder.m_node_count);
			m_transformations.resize(p_builder.m_node_count);
			m_bone_offsets.resize(p_builder.m_node_count);
			m_inverse_bind_poses.resize(p_builder.m_node_count);
			m_meshes.resize(p_builder.m_mesh_part_count);
			m_render_states.resize(p_builder.m_mesh_part_count);

			auto l_mesh_count = 0U;
			
			for(auto l_node_index = 0U; l_node_index < p_builder.m_nodes.size(); ++l_node_index)
			{
				bc_mesh_builder_mesh_node& l_builder_node = p_builder.m_nodes[l_node_index];
				bc_mesh_node& l_node = m_nodes[l_node_index];
				
				auto l_mesh_node_parent_ite = std::find_if(std::begin(m_nodes), std::end(m_nodes), [&p_builder, &l_builder_node](bc_mesh_node& p_node)
				{
					if(l_builder_node.m_parent_index == -1)
					{
						return false;
					}
					return std::strcmp(p_node.m_name.c_str(), p_builder.m_nodes[l_builder_node.m_parent_index].m_name.c_str()) == 0;
				});

				l_node.m_name = l_builder_node.m_name.c_str();
				l_node.m_parent = l_mesh_node_parent_ite != std::end(m_nodes) ? &*l_mesh_node_parent_ite : nullptr;
				l_node.m_transform_index = l_node_index;
				l_node.m_first_mesh_index = l_mesh_count;
				l_node.m_mesh_count = l_builder_node.m_mesh_parts.size();

				m_transformations[l_node_index] = l_builder_node.m_transform;
				m_bone_offsets[l_node_index] = l_builder_node.m_bone_offset;

				for(auto l_builder_node_mesh_ite = 0U; l_builder_node_mesh_ite < l_builder_node.m_mesh_parts.size(); ++l_builder_node_mesh_ite)
				{
					const auto l_mesh_index = l_node.m_first_mesh_index + l_builder_node_mesh_ite;
					
					bc_mesh_builder_mesh_part& l_builder_mesh = l_builder_node.m_mesh_parts[l_builder_node_mesh_ite];
					bc_mesh_part_data& l_mesh = m_meshes[l_mesh_index];
					
					l_mesh.m_name = l_builder_mesh.m_name.c_str();
					l_mesh.m_material = std::move(l_builder_mesh.m_material);
					l_mesh.m_vertices = std::move(l_builder_mesh.m_vertices);
					l_mesh.m_skinned_vertices = std::move(l_builder_mesh.m_skinned_vertices);
					l_mesh.m_indices = std::move(l_builder_mesh.m_indices);
					l_mesh.m_bound_box = l_builder_mesh.m_bound_box;
					l_mesh.m_vertex_buffer = std::move(l_builder_mesh.m_vertex_buffer);
					l_mesh.m_index_buffer = std::move(l_builder_mesh.m_index_buffer);
					
					m_render_states[l_mesh_index] = std::move(l_builder_mesh.m_render_state);

					auto* l_mesh_colliders = m_colliders->find_mesh_colliders(m_meshes[l_mesh_index].m_name);
					m_colliders_map.push_back(l_mesh_colliders);
				}
				
				m_nodes_map.insert(std::make_pair(hash_t()(l_node.m_name.c_str()), &l_node));
				if(l_node.m_parent == nullptr)
				{
					m_root = &l_node;
				}
				if(l_mesh_node_parent_ite != std::end(m_nodes))
				{
					l_mesh_node_parent_ite->m_children.push_back(&l_node);
				}

				l_mesh_count += l_builder_node.m_mesh_parts.size();
			}

			if(p_builder.m_scale.is_set())
			{
				_apply_auto_scale(*p_builder.m_scale);
			}
			
			auto l_identity = core::bc_matrix4f::identity();
			iterate_over_nodes(l_identity, [this](const bc_mesh_node& p_node, core::bc_matrix4f& p_parent_transform)
			{
				auto l_node_absolute_transformation = m_transformations[p_node.m_transform_index] * p_parent_transform;
				//m_inverse_bind_poses[p_node.m_transform_index] = m_transformations[m_root->m_transform_index].inverse() * l_node_absolute_transformation * m_bone_offsets[p_node.m_transform_index];
				//m_inverse_bind_poses[p_node.m_transform_index] = p_parent_transform * m_bone_offsets[p_node.m_transform_index] * m_transformations[p_node.m_transform_index];
				m_inverse_bind_poses[p_node.m_transform_index] = l_node_absolute_transformation.inverse();

				return l_node_absolute_transformation;
			});
		}

		const bc_mesh_node* bc_mesh::find_node(const bcCHAR* p_name) const noexcept
		{
			const auto l_hash = hash_t()(p_name);
			const auto l_entry = m_nodes_map.find(l_hash);

			if(l_entry == std::end(m_nodes_map))
			{
				return nullptr;
			}

			return l_entry->second;
		}

		const bc_mesh_node* bc_mesh::get_node_parent(const bc_mesh_node& p_node) const noexcept
		{
			return p_node.m_parent;
		}

		const core::bc_vector<bc_mesh_node*>& bc_mesh::get_node_children(const bc_mesh_node& p_node) const noexcept
		{
			return p_node.m_children;
		}

		const core::bc_matrix4f& bc_mesh::get_node_offset_transform(const bc_mesh_node& p_node) const noexcept
		{
			return m_bone_offsets[p_node.m_transform_index];
		}

		const core::bc_matrix4f& bc_mesh::get_node_transform(const bc_mesh_node& p_node) const noexcept
		{
			return m_transformations[p_node.m_transform_index];
		}

		const core::bc_string& bc_mesh::get_node_mesh_name(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			if (p_mesh_index >= p_node.m_mesh_count)
			{
				throw bc_out_of_range_exception("Invalid mesh index");
			}

			return m_meshes[p_node.m_first_mesh_index + p_mesh_index].m_name;
		}

		const bc_render_material& bc_mesh::get_node_mesh_material(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			if(p_mesh_index >= p_node.m_mesh_count)
			{
				throw bc_out_of_range_exception("Invalid mesh index");
			}

			return *m_meshes[p_node.m_first_mesh_index + p_mesh_index].m_material;
		}

		const bc_render_state& bc_mesh::get_node_mesh_render_state(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			if (p_mesh_index >= p_node.m_mesh_count)
			{
				throw bc_out_of_range_exception("Invalid mesh index");
			}

			return *m_render_states[p_node.m_first_mesh_index + p_mesh_index];
		}

		bc_render_state_ptr bc_mesh::get_node_mesh_render_state_ptr(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			if (p_mesh_index >= p_node.m_mesh_count)
			{
				throw bc_out_of_range_exception("Invalid mesh index");
			}

			return m_render_states[p_node.m_first_mesh_index + p_mesh_index];
		}

		const physics::bc_bound_box& bc_mesh::get_node_mesh_bound_box(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			if (p_mesh_index >= p_node.m_mesh_count)
			{
				throw bc_out_of_range_exception("Invalid mesh index");
			}

			return m_meshes[p_node.m_first_mesh_index + p_mesh_index].m_bound_box;
		}

		const bc_mesh_part_collider& bc_mesh::get_node_mesh_colliders(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			if (p_mesh_index >= p_node.m_mesh_count)
			{
				throw bc_out_of_range_exception("Invalid mesh index");
			}

			return *m_colliders_map[p_node.m_first_mesh_index + p_mesh_index];
		}

		void bc_mesh::calculate_bound_box(const bc_sub_mesh_transform& p_absolute_transforms, physics::bc_bound_box& p_bound_box) const noexcept
		{
			p_bound_box.set_empty();

			int l_dummy = 0;
			iterate_over_nodes(l_dummy, [this, &p_absolute_transforms, &p_bound_box](const bc_mesh_node& p_node, int)
			{
				const auto& l_node_transform = p_absolute_transforms.get_node_transform(p_node);

				// Update mesh bounding box based on its sub meshes
				for (bcSIZE m = 0; m < p_node.get_mesh_count(); ++m)
				{
					auto l_node_mesh_box = get_node_mesh_bound_box(p_node, m);
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

		void bc_mesh::calculate_absolute_transforms(const core::bc_matrix4f& p_world, bc_sub_mesh_transform& p_transforms, physics::bc_bound_box& p_bound_box) const noexcept
		{
			p_bound_box.set_empty();

			iterate_over_nodes(p_world, [this, &p_transforms, &p_bound_box](const bc_mesh_node& p_node, const core::bc_matrix4f& p_parent_transform)
			{
				const auto l_node_absolute_transformation = get_node_transform(p_node) * p_parent_transform;
				p_transforms.set_node_transformation
				(
					p_node,
					m_scale == 1
					? l_node_absolute_transformation
					: core::bc_matrix4f::scale_matrix(m_scale) * l_node_absolute_transformation
				);

				// Update mesh bounding box based on its sub meshes
				for (bcSIZE l_m = 0; l_m < p_node.get_mesh_count(); ++l_m)
				{
					auto l_node_mesh_box = get_node_mesh_bound_box(p_node, l_m);
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

		void bc_mesh::_apply_auto_scale(bcFLOAT p_scale)
		{
			bc_sub_mesh_transform l_transformations(*m_root);
			physics::bc_bound_box l_bound_box;
			l_bound_box.set_empty();

			calculate_absolute_transforms(core::bc_matrix4f::identity(), l_transformations, l_bound_box);

			const bcFLOAT l_largest_side = std::max
			(
				l_bound_box.get_half_extends().x,
				std::max
				(
					l_bound_box.get_half_extends().y,
					l_bound_box.get_half_extends().z
				)
			) * 2;

			m_scale = p_scale / l_largest_side;

			for(auto& l_mesh_part_data : m_meshes)
			{
				l_mesh_part_data.m_bound_box.scale(m_scale);
			}

			for(auto l_mesh_part_collider_ite = m_colliders->cbegin(); l_mesh_part_collider_ite != m_colliders->cend(); ++l_mesh_part_collider_ite)
			{
				for
				(
					auto l_mesh_part_collider_entry_ite = l_mesh_part_collider_ite->second.cbegin();
					l_mesh_part_collider_entry_ite != l_mesh_part_collider_ite->second.cend();
					++l_mesh_part_collider_entry_ite
				)
				{
					bc_mesh_part_collider_entry& l_entry = *l_mesh_part_collider_entry_ite;

					switch (l_entry.m_px_shape->get_type())
					{
					case physics::bc_shape_type::sphere:
						{
							auto& l_sphere = static_cast< physics::bc_shape_sphere& >(*l_entry.m_px_shape);
							l_sphere = physics::bc_shape_sphere(l_sphere.get_radius() * m_scale);
							break;
						}
					case physics::bc_shape_type::plane:
						{
							auto& l_plane = static_cast< physics::bc_shape_plane& >(*l_entry.m_px_shape);
							l_plane = physics::bc_shape_plane(l_plane.get_normal(), l_plane.get_distance() * m_scale);
							break;
						}
					case physics::bc_shape_type::capsule:
						{
							auto& l_capsule = static_cast< physics::bc_shape_capsule& >(*l_entry.m_px_shape);
							l_capsule = physics::bc_shape_capsule(l_capsule.get_half_height() * m_scale, l_capsule.get_radius() * m_scale);
							break;
						}
					case physics::bc_shape_type::box:
						{
							auto& l_box = static_cast< physics::bc_shape_box& >(*l_entry.m_px_shape);
							l_box = physics::bc_shape_box(l_box.get_half_extends() * m_scale);
							break;
						}
					case physics::bc_shape_type::convex_mesh:
						{
							auto& l_convex = static_cast< physics::bc_shape_convex_mesh& >(*l_entry.m_px_shape);
							l_convex = physics::bc_shape_convex_mesh(physics::bc_geometry_scale(m_scale), l_convex.get_convex());
							break;
						}
					case physics::bc_shape_type::triangle_mesh:
						{
							auto& l_triangle = static_cast< physics::bc_shape_triangle_mesh& >(*l_entry.m_px_shape);
							l_triangle = physics::bc_shape_triangle_mesh(physics::bc_geometry_scale(m_scale), l_triangle.get_mesh());
							break;
						}
					case physics::bc_shape_type::height_field:
						{
							BC_ASSERT(false);
							break;
						}
					default:;
					}
				}
			}
		}
	}
}