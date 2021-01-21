// [01/18/2017 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcIteratorAdapter.h"
#include "PhysicsImp/Fundation/bcTransform.h"
#include "PhysicsImp/Shape/bcShape.h"
#include "Game/Object/Mesh/bcMeshNode.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		struct bc_mesh_part_collider_entry
		{
			bc_mesh_part_collider_entry(core::bc_unique_ptr< physics::bc_shape_geometry > p_px_shape,
				bc_mesh_node::node_index_t p_attached_node_transform_index,
				const physics::bc_transform& p_local_transform,
				const physics::bc_transform& p_model_transform)
				: bc_mesh_part_collider_entry
				(
					std::move(p_px_shape),
					p_attached_node_transform_index,
					p_local_transform,
					p_model_transform,
					physics::bc_shape_flag::default_v
				)
			{
			}

			bc_mesh_part_collider_entry(core::bc_unique_ptr< physics::bc_shape_geometry > p_px_shape,
				bc_mesh_node::node_index_t p_attached_node_transform_index,
				const physics::bc_transform& p_local_transform,
				const physics::bc_transform& p_model_transform,
				physics::bc_shape_flag p_flag)
				: m_px_shape(std::move(p_px_shape)),
				m_attached_node_transform_index(p_attached_node_transform_index),
				m_local_transform(p_local_transform),
				m_model_transform(p_model_transform),
				m_shape_flags(p_flag)
			{
			}

			bc_mesh_part_collider_entry(bc_mesh_part_collider_entry&&) = default;

			~bc_mesh_part_collider_entry() = default;

			bc_mesh_part_collider_entry& operator=(bc_mesh_part_collider_entry&&) = default;

			core::bc_unique_ptr< physics::bc_shape_geometry > m_px_shape;
			bc_mesh_node::node_index_t m_attached_node_transform_index;
			physics::bc_transform m_local_transform;
			physics::bc_transform m_model_transform;
			physics::bc_shape_flag m_shape_flags;
		};

		class BC_GAME_DLL bc_mesh_part_collider : public core::bc_const_iterator_adapter<core::bc_vector<bc_mesh_part_collider_entry>>
		{
		public:
			using entry = bc_mesh_part_collider_entry;

		public:
			bc_mesh_part_collider();

			bc_mesh_part_collider(bc_mesh_part_collider&&) noexcept;

			~bc_mesh_part_collider();

			bc_mesh_part_collider& operator=(bc_mesh_part_collider&&) noexcept;

			void add_px_shape(const physics::bc_shape_box& p_box,
				bc_mesh_node::node_index_t p_attached_node_transform_index,
				const physics::bc_transform& p_local_transform, 
				physics::bc_shape_flag p_flags = physics::bc_shape_flag::default_v);

			void add_px_shape(const physics::bc_shape_sphere& p_sphere,
				bc_mesh_node::node_index_t p_attached_node_transform_index,
				const physics::bc_transform& p_local_transform, 
				physics::bc_shape_flag p_flags = physics::bc_shape_flag::default_v);

			void add_px_shape(const physics::bc_shape_capsule& p_capsule,
				bc_mesh_node::node_index_t p_attached_node_transform_index,
				const physics::bc_transform& p_local_transform, 
				physics::bc_shape_flag p_flags = physics::bc_shape_flag::default_v);

			void add_px_shape(physics::bc_convex_mesh_ref&& p_convex,
				bc_mesh_node::node_index_t p_attached_node_transform_index,
				const physics::bc_transform& p_local_transform, 
				physics::bc_shape_flag p_flags = physics::bc_shape_flag::default_v);

			void add_px_shape(physics::bc_triangle_mesh_ref&& p_mesh,
				bc_mesh_node::node_index_t p_attached_node_transform_index,
				const physics::bc_transform& p_local_transform, 
				physics::bc_shape_flag p_flags = physics::bc_shape_flag::default_v);

			void shrink_to_fit();

		private:
			core::bc_vector< bc_mesh_part_collider_entry > m_shapes;
			core::bc_vector< physics::bc_convex_mesh_ref > m_convex_shapes;
			core::bc_vector< physics::bc_triangle_mesh_ref > m_triangle_shapes;
		};
	}
}