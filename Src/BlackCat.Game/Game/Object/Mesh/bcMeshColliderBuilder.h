// [11/19/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"
#include "PhysicsImp/Shape/bcShape.h"
#include "Game/Object/Mesh/bcMeshNode.h"
#include "Game/Object/Mesh/bcMeshCollider.h"
#include "Game/Object/Mesh/bcSkinnedMeshCollider.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_mesh_collider_builder
		{
			friend class bc_mesh_collider;
			
		public:
			bc_mesh_collider_builder() noexcept;

			bc_mesh_collider_builder(bc_mesh_collider_builder&&) noexcept = default;

			~bc_mesh_collider_builder() = default;

			bc_mesh_collider_builder& operator=(bc_mesh_collider_builder&&) noexcept = default;
			
			void add_px_shape(const bcCHAR* p_attached_mesh_name,
				bc_mesh_node::node_index_t p_attached_node_index,
				const physics::bc_shape_box& p_box,
				const physics::bc_transform& p_local_transform,
				physics::bc_shape_flag p_flags = physics::bc_shape_flag::default_v);

			void add_px_shape(const bcCHAR* p_attached_mesh_name,
				bc_mesh_node::node_index_t p_attached_node_index,
				const physics::bc_shape_sphere& p_sphere,
				const physics::bc_transform& p_local_transform,
				physics::bc_shape_flag p_flags = physics::bc_shape_flag::default_v);

			void add_px_shape(const bcCHAR* p_attached_mesh_name,
				bc_mesh_node::node_index_t p_attached_node_index,
				const physics::bc_shape_capsule& p_capsule,
				const physics::bc_transform& p_local_transform,
				physics::bc_shape_flag p_flags = physics::bc_shape_flag::default_v);

			void add_px_shape(const bcCHAR* p_attached_mesh_name,
				bc_mesh_node::node_index_t p_attached_node_index,
				physics::bc_convex_mesh_ref&& p_convex,
				const physics::bc_transform& p_local_transform,
				physics::bc_shape_flag p_flags = physics::bc_shape_flag::default_v);

			void add_px_shape(const bcCHAR* p_attached_mesh_name,
				bc_mesh_node::node_index_t p_attached_node_index,
				physics::bc_triangle_mesh_ref&& p_mesh,
				const physics::bc_transform& p_local_transform,
				physics::bc_shape_flag p_flags = physics::bc_shape_flag::default_v,
				bool p_high_detail_query_shape = false);

			void add_px_joint(std::string_view p_collider1,
				std::string_view p_collider2,
				const physics::bc_transform& p_transform);
			
			void add_skinned_mesh_collider(bc_skinned_mesh_collider_vertex* p_vertices, 
				bcUINT16* p_indices, 
				bcSIZE p_vertices_count, 
				bcSIZE p_faces_count);

			void add_skinned_mesh_collider(bc_skinned_mesh_collider_vertex* p_vertices, 
				bcUINT32* p_indices, 
				bcSIZE p_vertices_count, 
				bcSIZE p_faces_count);
			
			bc_mesh_collider build();

		private:
			core::bc_vector<bc_mesh_part_collider_entry>& _get_mesh_colliders(const bcCHAR* p_mesh_name);
			
			core::bc_unordered_map<const bcCHAR*, core::bc_vector<bc_mesh_part_collider_entry>> m_mesh_colliders;
			core::bc_vector<physics::bc_convex_mesh_ref> m_convex_shapes;
			core::bc_vector<physics::bc_triangle_mesh_ref> m_triangle_shapes;
			core::bc_vector<bc_mesh_part_collider_joint_entry> m_joints;

			bcSIZE m_skinned_faces_count;
			core::bc_vector_movable<bc_skinned_mesh_collider_vertex> m_skinned_vertices;
			core::bc_vector_movable<bcUINT16> m_skinned_16bit_indices;
			core::bc_vector_movable<bcUINT32> m_skinned_32bit_indices;
		};
	}
}