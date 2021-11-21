// [11/19/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcMeshColliderBuilder.h"

namespace black_cat
{
	namespace game
	{
		bc_mesh_collider_builder::bc_mesh_collider_builder() noexcept
			: m_skinned_faces_count(0)
		{
		}

		void bc_mesh_collider_builder::add_px_shape(const bcCHAR* p_attached_mesh_name,
			bc_mesh_node::node_index_t p_attached_node_index,
			const physics::bc_shape_box& p_box,
			const physics::bc_transform& p_local_transform,
			physics::bc_shape_flag p_flags)
		{
			auto& l_mesh_colliders = _get_mesh_colliders(p_attached_mesh_name);
			l_mesh_colliders.push_back
			(
				bc_mesh_part_collider_entry
				(
					p_attached_node_index,
					p_attached_mesh_name,
					core::bc_make_unique<physics::bc_shape_box>(p_box),
					p_local_transform,
					p_flags
				)
			);
		}

		void bc_mesh_collider_builder::add_px_shape(const bcCHAR* p_attached_mesh_name,
			bc_mesh_node::node_index_t p_attached_node_index,
			const physics::bc_shape_sphere& p_sphere,
			const physics::bc_transform& p_local_transform,
			physics::bc_shape_flag p_flags)
		{
			auto& l_mesh_colliders = _get_mesh_colliders(p_attached_mesh_name);
			l_mesh_colliders.push_back
			(
				bc_mesh_part_collider_entry
				(
					p_attached_node_index,
					p_attached_mesh_name,
					core::bc_make_unique<physics::bc_shape_sphere>(p_sphere),
					p_local_transform,
					p_flags
				)
			);
		}

		void bc_mesh_collider_builder::add_px_shape(const bcCHAR* p_attached_mesh_name,
			bc_mesh_node::node_index_t p_attached_node_index,
			const physics::bc_shape_capsule& p_capsule,
			const physics::bc_transform& p_local_transform,
			physics::bc_shape_flag p_flags)
		{
			auto& l_mesh_colliders = _get_mesh_colliders(p_attached_mesh_name);
			l_mesh_colliders.push_back
			(
				bc_mesh_part_collider_entry
				(
					p_attached_node_index,
					p_attached_mesh_name,
					core::bc_make_unique<physics::bc_shape_capsule>(p_capsule),
					p_local_transform,
					p_flags
				)
			);
		}

		void bc_mesh_collider_builder::add_px_shape(const bcCHAR* p_attached_mesh_name,
			bc_mesh_node::node_index_t p_attached_node_index,
			physics::bc_convex_mesh_ref&& p_convex,
			const physics::bc_transform& p_local_transform,
			physics::bc_shape_flag p_flags)
		{
			auto& l_mesh_colliders = _get_mesh_colliders(p_attached_mesh_name);
			l_mesh_colliders.push_back
			(
				bc_mesh_part_collider_entry
				(
					p_attached_node_index,
					p_attached_mesh_name,
					core::bc_make_unique<physics::bc_shape_convex_mesh>(physics::bc_shape_convex_mesh(p_convex.get())),
					p_local_transform,
					p_flags
				)
			);
			m_convex_shapes.push_back(std::move(p_convex));
		}

		void bc_mesh_collider_builder::add_px_shape(const bcCHAR* p_attached_mesh_name,
			bc_mesh_node::node_index_t p_attached_node_index,
			physics::bc_triangle_mesh_ref&& p_mesh,
			const physics::bc_transform& p_local_transform,
			physics::bc_shape_flag p_flags,
			bool p_high_detail_query_shape)
		{
			auto& l_mesh_colliders = _get_mesh_colliders(p_attached_mesh_name);
			l_mesh_colliders.push_back
			(
				bc_mesh_part_collider_entry
				(
					p_attached_node_index,
					p_attached_mesh_name,
					core::bc_make_unique<physics::bc_shape_triangle_mesh>(physics::bc_shape_triangle_mesh(p_mesh.get())),
					p_local_transform,
					p_flags,
					p_high_detail_query_shape
				)
			);
			m_triangle_shapes.push_back(std::move(p_mesh));
		}

		void bc_mesh_collider_builder::add_skinned_mesh_collider(bc_skinned_mesh_collider_vertex* p_vertices,
			bcUINT16* p_indices,
			bcSIZE p_vertices_count,
			bcSIZE p_faces_count)
		{
			const auto l_vertices_count = m_skinned_vertices.size();
			const auto l_indices_count = m_skinned_16bit_indices.size();

			if (l_indices_count + p_faces_count * 3 > std::numeric_limits<bcUINT16>::max())
			{
				throw bc_invalid_operation_exception("Skinned collider indices count is higher than 16bit indices limit");
			}

			m_skinned_faces_count += p_faces_count;
			m_skinned_vertices.resize(m_skinned_vertices.size() + p_vertices_count);
			m_skinned_16bit_indices.resize(m_skinned_16bit_indices.size() + p_faces_count * 3);

			std::memcpy(m_skinned_vertices.data() + l_vertices_count, p_vertices, p_vertices_count * sizeof(bc_skinned_mesh_collider_vertex));
			std::memcpy(m_skinned_16bit_indices.data() + l_indices_count, p_indices, p_faces_count * 3 * sizeof(bcUINT16));

			for (auto l_ite = l_indices_count; l_ite < m_skinned_16bit_indices.size(); ++l_ite)
			{
				m_skinned_16bit_indices[l_ite] += l_vertices_count;
			}
		}

		void bc_mesh_collider_builder::add_skinned_mesh_collider(bc_skinned_mesh_collider_vertex* p_vertices,
			bcUINT32* p_indices,
			bcSIZE p_vertices_count,
			bcSIZE p_faces_count)
		{
			if (!m_skinned_16bit_indices.empty())
			{
				m_skinned_32bit_indices.resize(m_skinned_16bit_indices.size());
				std::copy(std::begin(m_skinned_16bit_indices), std::end(m_skinned_16bit_indices), std::begin(m_skinned_32bit_indices));
				m_skinned_16bit_indices.clear();
				m_skinned_16bit_indices.shrink_to_fit();
			}

			const auto l_vertices_count = m_skinned_vertices.size();
			const auto l_indices_count = m_skinned_32bit_indices.size();

			m_skinned_faces_count += p_faces_count;
			m_skinned_vertices.resize(m_skinned_vertices.size() + p_vertices_count);
			m_skinned_32bit_indices.resize(m_skinned_32bit_indices.size() + p_faces_count * 3);

			std::memcpy(m_skinned_vertices.data() + l_vertices_count, p_vertices, p_vertices_count * sizeof(bc_skinned_mesh_collider_vertex));
			std::memcpy(m_skinned_32bit_indices.data() + l_indices_count, p_indices, p_faces_count * 3 * sizeof(bcUINT32));

			for (auto l_ite = l_indices_count; l_ite < m_skinned_32bit_indices.size(); ++l_ite)
			{
				m_skinned_32bit_indices[l_ite] += l_vertices_count;
			}
		}

		bc_mesh_collider bc_mesh_collider_builder::build()
		{
			bc_mesh_collider l_collider;
			l_collider.m_convex_shapes = std::move(m_convex_shapes);
			l_collider.m_triangle_shapes = std::move(m_triangle_shapes);

			l_collider.m_colliders.reserve(m_mesh_colliders.size());
			l_collider.m_mesh_colliders.reserve(m_mesh_colliders.size());
			
			for(auto& l_mesh_collider : m_mesh_colliders)
			{
				l_collider.m_mesh_colliders.push_back(std::make_tuple
				(
					core::bc_string(l_mesh_collider.first), 
					l_collider.m_mesh_colliders.size(), 
					l_mesh_collider.second.size()
				));
				std::move(std::begin(l_mesh_collider.second), std::end(l_mesh_collider.second), std::back_inserter(l_collider.m_colliders));
			}

			l_collider.m_skinned_collider.m_faces_count = m_skinned_faces_count;
			l_collider.m_skinned_collider.m_vertices = std::move(m_skinned_vertices);
			l_collider.m_skinned_collider.m_16bit_indices = std::move(m_skinned_16bit_indices);
			l_collider.m_skinned_collider.m_32bit_indices = std::move(m_skinned_32bit_indices);

			l_collider.m_mesh_colliders.shrink_to_fit();
			l_collider.m_colliders.shrink_to_fit();
			l_collider.m_convex_shapes.shrink_to_fit();
			l_collider.m_triangle_shapes.shrink_to_fit();
			l_collider.m_skinned_collider.m_vertices.shrink_to_fit();
			l_collider.m_skinned_collider.m_16bit_indices.shrink_to_fit();
			l_collider.m_skinned_collider.m_32bit_indices.shrink_to_fit();
			
			return l_collider;
		}

		core::bc_vector<bc_mesh_part_collider_entry>& bc_mesh_collider_builder::_get_mesh_colliders(const bcCHAR* p_mesh_name)
		{
			auto l_ite = m_mesh_colliders.find(p_mesh_name);
			if (l_ite == std::end(m_mesh_colliders))
			{
				l_ite = m_mesh_colliders.insert(std::make_pair(p_mesh_name, core::bc_vector<bc_mesh_part_collider_entry>())).first;
			}

			return l_ite->second;
		}
	}
}