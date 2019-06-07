// [06/14/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/bcException.h"
#include "Game/Object/Mesh/bcMesh.h"
#include "Game/Object/Mesh/bcMeshPartCollider.h"
#include "Game/Object/Mesh/bcSubMeshTransformation.h"

namespace black_cat
{
	namespace game
	{
		// -- bc_mesh_node --------------------------------------------------------------------------------

		bc_mesh_node::bc_mesh_node(core::bc_string p_name,
			bc_mesh_node* p_parent,
			node_index p_transformation_index,
			node_index p_first_mesh_index,
			bcSIZE p_mesh_count)
			: m_name(std::move(p_name)),
			m_parent(p_parent),
			m_transformation_index(p_transformation_index),
			m_first_mesh_index(p_first_mesh_index),
			m_mesh_count(p_mesh_count)
		{
		}

		bc_mesh_node::node_index bc_mesh_node::get_transformation_index() const
		{
			return m_transformation_index;
		}

		bc_mesh_node::node_index bc_mesh_node::get_mesh_count() const
		{
			return m_mesh_count;
		}

		bcSIZE bc_mesh_node::get_children_count() const
		{
			return m_children.size();
		}

		bcSIZE bc_mesh_node::get_all_children_count() const
		{
			bcUINT32 l_children_count = m_children.size();

			for(auto* l_child : m_children)
			{
				l_children_count += l_child->get_all_children_count();
			}

			return l_children_count;
		}

		const core::bc_string& bc_mesh_node::get_name() const
		{
			return m_name;
		}

		void bc_mesh_node::_add_child(bc_mesh_node* p_mesh_node)
		{
			m_children.push_back(p_mesh_node);
		}

		// -- bc_mesh --------------------------------------------------------------------------------

		bc_mesh::bc_mesh(core::bc_string p_name, bcSIZE p_node_count, bcSIZE p_mesh_count, bc_mesh_collider_ptr p_colliders)
			: m_name(std::move(p_name)),
			m_root(nullptr),
			m_colliders(std::move(p_colliders)),
			m_scale(1)
		{
			// Reserve needed memory for nodes because we use raw pointers in bc_mesh_node for parent and children
			m_nodes.reserve(p_node_count);
			m_nodes_map.reserve(p_node_count);
			m_transformations.reserve(p_node_count);
			m_meshes.reserve(p_mesh_count);
			m_render_states.reserve(p_mesh_count);
		}

		const core::bc_string& bc_mesh::get_name() const
		{
			return m_name;
		}

		const bc_mesh_node* bc_mesh::get_root() const
		{
			return m_root;
		}

		const bc_mesh_node* bc_mesh::find_node(const bcCHAR* p_name) const
		{
			auto l_hash = hash_t()(p_name);
			auto l_entry = m_nodes_map.find(l_hash);

			if(l_entry == std::end(m_nodes_map))
			{
				return nullptr;
			}

			return l_entry->second;
		}

		const bc_mesh_node* bc_mesh::get_node_parent(const bc_mesh_node* p_node) const
		{
			return p_node->m_parent;
		}

		const core::bc_vector<bc_mesh_node*>& bc_mesh::get_node_children(const bc_mesh_node* p_node) const
		{
			return p_node->m_children;
		}

		const core::bc_matrix4f& bc_mesh::get_node_transformation(const bc_mesh_node* p_node) const
		{
			return m_transformations.at(p_node->m_transformation_index);
		}

		const bc_render_material& bc_mesh::get_node_mesh_material(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const
		{
			if(p_mesh_index >= p_node->m_mesh_count)
			{
				throw bc_out_of_range_exception("Invalid mesh index");
			}

			return *m_meshes[p_node->m_first_mesh_index + p_mesh_index].m_material;
		}

		const bc_render_state& bc_mesh::get_node_mesh_render_state(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const
		{
			if (p_mesh_index >= p_node->m_mesh_count)
			{
				throw bc_out_of_range_exception("Invalid mesh index");
			}

			return *m_render_states[p_node->m_first_mesh_index + p_mesh_index];
		}

		const physics::bc_bound_box& bc_mesh::get_node_mesh_bound_box(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const
		{
			if (p_mesh_index >= p_node->m_mesh_count)
			{
				throw bc_out_of_range_exception("Invalid mesh index");
			}

			return m_meshes[p_node->m_first_mesh_index + p_mesh_index].m_bound_box;
		}

		const bc_mesh_part_collider& bc_mesh::get_node_mesh_colliders(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const
		{
			if (p_mesh_index >= p_node->m_mesh_count)
			{
				throw bc_out_of_range_exception("Invalid mesh index");
			}

			return *m_colliders_map[p_node->m_first_mesh_index + p_mesh_index];;
		}

		void bc_mesh::calculate_absolute_transformations(const core::bc_matrix4f& p_world, bc_sub_mesh_transformation& p_result, physics::bc_bound_box& p_bound_box) const
		{
			p_bound_box.set_empty();

			const bc_mesh_node* l_begin = &m_nodes[p_result.get_root_node_index()];

			_calculate_absolute_transformations(p_world, l_begin, l_begin + 1, p_result, p_bound_box);
		}

		bc_mesh_node* bc_mesh::_add_node(core::bc_string p_name,
			bc_mesh_node* p_parent,
			core::bc_matrix4f& p_transformation,
			const core::bc_vector_frame<std::tuple<bc_mesh_part_data, bc_render_state_ptr>>& p_meshes)
		{
			if(!p_parent)
			{
				m_nodes.clear();
				m_nodes_map.clear();
				m_transformations.clear();
				m_meshes.clear();
				m_render_states.clear();
				m_colliders_map.clear();
			}

			auto l_node_hash = hash_t()(p_name.c_str());
			const auto l_transformation_index = m_transformations.size();
			const auto l_mesh_start_index = m_meshes.size();
			const auto l_mesh_count = p_meshes.size();
			
			m_nodes.push_back(bc_mesh_node(std::move(p_name), p_parent, l_transformation_index, l_mesh_start_index, l_mesh_count));
			m_nodes_map.insert(std::make_pair(l_node_hash, &*m_nodes.rbegin()));
			m_transformations.push_back(p_transformation);

			for (auto& l_mesh : p_meshes)
			{
				const auto* l_mesh_part_colliders = m_colliders->find_mesh_colliders(std::get<bc_mesh_part_data>(l_mesh).m_name);

				m_meshes.push_back(std::move(std::get<bc_mesh_part_data>(l_mesh)));
				m_render_states.push_back(std::move(std::get<bc_render_state_ptr>(l_mesh)));
				m_colliders_map.push_back(l_mesh_part_colliders);
			}

			auto l_node = &(*m_nodes.rbegin());

			if(!p_parent)
			{
				m_root = l_node;
			}
			else
			{
				p_parent->_add_child(l_node);
			}

			return l_node;
		}

		void bc_mesh::_apply_auto_scale(bcFLOAT p_scale)
		{
			bc_sub_mesh_transformation l_transformations(*m_root);
			physics::bc_bound_box l_bound_box;
			l_bound_box.set_empty();

			calculate_absolute_transformations(core::bc_matrix4f::identity(), l_transformations, l_bound_box);

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
							bcAssert(false);
							break;
						}
					default:;
					}
				}
			}
		}

		void bc_mesh::_calculate_absolute_transformations(const core::bc_matrix4f& p_parent_transformation,
			const bc_mesh_node* p_begin,
			const bc_mesh_node* p_end,
			bc_sub_mesh_transformation& p_result,
			physics::bc_bound_box& p_bound_box) const
		{
			for (; p_begin != p_end; ++p_begin)
			{
				const bc_mesh_node* l_node = p_begin;

				auto l_node_absolute_transformation = get_node_transformation(l_node) * p_parent_transformation;
				p_result.set_node_transformation
				(
					*l_node,
					m_scale == 1
						? l_node_absolute_transformation
						: core::bc_matrix4f::scale_matrix(m_scale) * l_node_absolute_transformation
				);

				// Update mesh bounding box based on its sub meshes
				for (bcSIZE m = 0; m < l_node->get_mesh_count(); ++m)
				{
					auto l_node_mesh_box = get_node_mesh_bound_box(l_node, m);
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

				const auto& l_node_children = get_node_children(l_node);
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