// [06/14/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/bcException.h"
#include "Game/Object/Mesh/bcMesh.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/Object/Mesh/bcMeshBuilder.h"
#include "Game/Object/Mesh/bcMeshCollider.h"
#include "Game/Object/Mesh/bcMeshLevelOfDetail.h"
#include "Game/Object/Mesh/bcMeshUtility.h"
#include "Game/Object/Mesh/bcSubMeshTransform.h"

namespace black_cat
{
	namespace game
	{
		bc_mesh::bc_mesh(bc_mesh_builder p_builder, bc_mesh_collider_ptr p_colliders)
			: m_name(std::move(p_builder.m_name)),
			m_auto_scale(0),
			m_scale(1),
			m_skinned(p_builder.m_skinned),
			m_root(nullptr),
			m_collider(std::move(p_colliders))
		{
			// Reserve needed memory for nodes because we use raw pointers in bc_mesh_node for parent and children
			m_nodes.resize(p_builder.m_node_count);
			m_nodes_map.reserve(p_builder.m_node_count);
			m_transformations.resize(p_builder.m_node_count);
			m_bind_poses.resize(p_builder.m_node_count);
			m_inverse_bind_poses.resize(p_builder.m_node_count);
			m_meshes.resize(p_builder.m_mesh_part_count);
			m_render_states.resize(p_builder.m_mesh_part_count);
			m_level_of_details.resize(p_builder.m_level_of_details.size());
			m_level_of_details_map.resize(p_builder.m_level_of_details.size() + 1);

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
				l_node.m_index = l_node_index;
				l_node.m_first_mesh_index = l_mesh_count;
				l_node.m_mesh_count = l_builder_node.m_mesh_parts.size();

				m_transformations[l_node_index] = l_builder_node.m_transform;

				for(auto l_builder_node_mesh_ite = 0U; l_builder_node_mesh_ite < l_builder_node.m_mesh_parts.size(); ++l_builder_node_mesh_ite)
				{
					const auto l_mesh_index = l_node.m_first_mesh_index + l_builder_node_mesh_ite;
					
					bc_mesh_builder_mesh_part& l_builder_mesh = l_builder_node.m_mesh_parts[l_builder_node_mesh_ite];
					bc_mesh_part_data& l_mesh = m_meshes[l_mesh_index];
					
					l_mesh.m_name = l_builder_mesh.m_name;
					l_mesh.m_material = std::move(l_builder_mesh.m_material);
					l_mesh.m_vertices = std::move(l_builder_mesh.m_vertices);
					l_mesh.m_skinned_vertices = std::move(l_builder_mesh.m_skinned_vertices);
					l_mesh.m_16bit_indices = std::move(l_builder_mesh.m_16bit_indices);
					l_mesh.m_32bit_indices = std::move(l_builder_mesh.m_32bit_indices);
					l_mesh.m_bound_box = l_builder_mesh.m_bound_box;
					l_mesh.m_vertex_buffer = std::move(l_builder_mesh.m_vertex_buffer);
					l_mesh.m_index_buffer = std::move(l_builder_mesh.m_index_buffer);
					
					m_render_states[l_mesh_index] = std::move(l_builder_mesh.m_render_state);
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

			std::copy
			(
				std::begin(p_builder.m_level_of_details),
				std::end(p_builder.m_level_of_details),
				std::begin(m_level_of_details)
			);
			m_level_of_details_map[0] = this;
			std::transform
			(
				std::begin(p_builder.m_level_of_details),
				std::end(p_builder.m_level_of_details),
				std::begin(m_level_of_details_map) + 1,
				[](const bc_mesh_ptr& p_mesh)
				{
					return p_mesh.get();
				}
			);

			// It is important to calculate inverse bind poses matrices before applying auto-scale
			// to avoid reversing auto-scale
			_calculate_inverse_bind_pose();
			
			if (p_builder.m_auto_scale.has_value())
			{
				_apply_auto_scale(*p_builder.m_auto_scale);
			}

			_calculate_collider_absolute_transforms();

			// For unknown error in mesh builder deconstruction
			p_builder.m_nodes.clear();
		}

		bc_mesh::bc_mesh(bc_mesh&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		bc_mesh& bc_mesh::operator=(bc_mesh&& p_other) noexcept
		{
			m_name = std::move(p_other.m_name);
			m_auto_scale = p_other.m_auto_scale;
			m_scale = p_other.m_scale;
			m_skinned = p_other.m_skinned;
			m_root = p_other.m_root;
			m_nodes = std::move(p_other.m_nodes);
			m_nodes_map = std::move(p_other.m_nodes_map);
			m_transformations = std::move(p_other.m_transformations);
			m_bind_poses = std::move(p_other.m_bind_poses);
			m_inverse_bind_poses = std::move(p_other.m_inverse_bind_poses);
			m_meshes = std::move(p_other.m_meshes);
			m_render_states = std::move(p_other.m_render_states);
			m_collider = std::move(p_other.m_collider);
			m_level_of_details = std::move(p_other.m_level_of_details);
			m_level_of_details_map = std::move(p_other.m_level_of_details_map);
			m_level_of_details_map[0] = this;
			
			return *this;
		}

		const bc_mesh_node* bc_mesh::find_node(bc_mesh_node::node_index_t p_index) const noexcept
		{
			if(p_index >= m_nodes.size())
			{
				return nullptr;
			}
			
			return &m_nodes[p_index];
		}
		
		const bc_mesh_node* bc_mesh::find_node(core::bc_string_view p_name) const noexcept
		{
			const auto l_hash = hash_t()(p_name);
			const auto l_entry = m_nodes_map.find(l_hash);

			if(l_entry == std::end(m_nodes_map))
			{
				return nullptr;
			}

			return l_entry->second;
		}

		core::bc_string_view bc_mesh::get_node_mesh_name(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			if (p_mesh_index >= p_node.m_mesh_count)
			{
				throw bc_out_of_range_exception("Invalid mesh index");
			}

			const auto& l_mesh_name = m_meshes[p_node.m_first_mesh_index + p_mesh_index].m_name;
			return core::bc_string_view(l_mesh_name.c_str(), l_mesh_name.size());
		}

		const bc_mesh_material& bc_mesh::get_node_mesh_material(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			if(p_mesh_index >= p_node.m_mesh_count)
			{
				throw bc_out_of_range_exception("Invalid mesh index");
			}

			return *m_meshes[p_node.m_first_mesh_index + p_mesh_index].m_material;
		}

		bc_mesh_material_ptr bc_mesh::get_node_mesh_material_ptr(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			if (p_mesh_index >= p_node.m_mesh_count)
			{
				throw bc_out_of_range_exception("Invalid mesh index");
			}

			return m_meshes[p_node.m_first_mesh_index + p_mesh_index].m_material;
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

		core::bc_const_span<bc_mesh_part_collider_entry> bc_mesh::get_node_mesh_colliders(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			if (p_mesh_index >= p_node.m_mesh_count)
			{
				throw bc_out_of_range_exception("Invalid mesh index");
			}

			return m_collider->find_mesh_collider(get_node_mesh_name(p_node, p_mesh_index));
		}

		void bc_mesh::_apply_auto_scale(bcFLOAT p_auto_scale)
		{
			bc_sub_mesh_mat4_transform l_transformations(*m_root);
			physics::bc_bound_box l_bound_box;
			bc_mesh_utility::calculate_absolute_transforms(*this, core::bc_matrix4f::identity(), l_transformations, l_bound_box);

			const bcFLOAT l_largest_side = std::max({ l_bound_box.get_half_extends().x, l_bound_box.get_half_extends().y, l_bound_box.get_half_extends().z }) * 2;

			m_auto_scale = p_auto_scale;
			m_scale = p_auto_scale / l_largest_side;
			m_transformations[m_root->get_index()] *= core::bc_matrix4f::scale_matrix(m_scale);
			
			for(auto& l_mesh_part_data : m_meshes)
			{
				l_mesh_part_data.m_bound_box.scale(m_scale);
			}

			for(bc_mesh_part_collider_entry& l_collider : m_collider->get_colliders())
			{
				switch (l_collider.m_shape->get_type())
				{
				case physics::bc_shape_type::sphere:
					{
						auto& l_sphere = static_cast<physics::bc_shape_sphere&>(*l_collider.m_shape);
						l_sphere = physics::bc_shape_sphere(l_sphere.get_radius() * m_scale);
						break;
					}
				case physics::bc_shape_type::plane:
					{
						auto& l_plane = static_cast<physics::bc_shape_plane&>(*l_collider.m_shape);
						l_plane = physics::bc_shape_plane(l_plane.get_normal(), l_plane.get_distance() * m_scale);
						break;
					}
				case physics::bc_shape_type::capsule:
					{
						auto& l_capsule = static_cast<physics::bc_shape_capsule&>(*l_collider.m_shape);
						l_capsule = physics::bc_shape_capsule(l_capsule.get_half_height() * m_scale, l_capsule.get_radius() * m_scale);
						break;
					}
				case physics::bc_shape_type::box:
					{
						auto& l_box = static_cast<physics::bc_shape_box&>(*l_collider.m_shape);
						l_box = physics::bc_shape_box(l_box.get_half_extends() * m_scale);
						break;
					}
				case physics::bc_shape_type::convex_mesh:
					{
						auto& l_convex = static_cast<physics::bc_shape_convex_mesh&>(*l_collider.m_shape);
						l_convex = physics::bc_shape_convex_mesh(physics::bc_geometry_scale(m_scale), l_convex.get_convex());
						break;
					}
				case physics::bc_shape_type::triangle_mesh:
					{
						auto& l_triangle = static_cast<physics::bc_shape_triangle_mesh&>(*l_collider.m_shape);
						l_triangle = physics::bc_shape_triangle_mesh(physics::bc_geometry_scale(m_scale), l_triangle.get_mesh());
						break;
					}
				case physics::bc_shape_type::height_field:
					{
						BC_ASSERT(false);
						break;
					}
				default:
					{
						BC_ASSERT(false);
					}
				}

				l_collider.m_local_transform = physics::bc_transform
				(
					l_collider.m_local_transform.get_position() * m_scale,
					l_collider.m_local_transform.get_matrix3()
				);
			}

			for(bc_mesh_part_collider_joint_entry& l_joint : m_collider->get_collider_joints())
			{
				l_joint.m_transform = physics::bc_transform
				(
					l_joint.m_transform.get_position() * m_scale,
					l_joint.m_transform.get_matrix3()
				);
			}
		}

		void bc_mesh::_calculate_inverse_bind_pose()
		{
			auto l_identity = core::bc_matrix4f::identity();
			bc_mesh_utility::iterate_over_nodes
			(
				*this,
				l_identity, 
				[this](const bc_mesh_node& p_node, core::bc_matrix4f& p_parent_transform)
				{
					m_bind_poses[p_node.m_index] = get_node_transform(p_node) * p_parent_transform;
					m_inverse_bind_poses[p_node.m_index] = m_bind_poses[p_node.m_index].inverse();

					return m_bind_poses[p_node.m_index];
				},
				[](const bc_mesh_node&, const core::bc_matrix4f&)
				{
				}
			);
		}

		void bc_mesh::_calculate_collider_absolute_transforms()
		{
			physics::bc_bound_box l_bound_box;
			bc_sub_mesh_mat4_transform l_transforms(*get_root());
			bc_mesh_utility::calculate_absolute_transforms(*this, core::bc_matrix4f::identity(), l_transforms, l_bound_box);

			for (bc_mesh_part_collider_entry& l_collider : m_collider->get_colliders())
			{			
				if(m_skinned)
				{
					l_collider.m_absolute_transform = l_collider.m_local_transform;
				}
				else
				{
					auto& l_attached_node_transform = l_transforms[l_collider.m_attached_node_index];
					l_collider.m_absolute_transform = physics::bc_transform(l_attached_node_transform) * l_collider.m_local_transform;
				}
			}
		}
	}
}