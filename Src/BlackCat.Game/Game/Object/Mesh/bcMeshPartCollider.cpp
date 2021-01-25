// [01/21/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcMeshPartCollider.h"

namespace black_cat
{
	namespace game
	{
		bc_mesh_part_collider::bc_mesh_part_collider()
			: bc_const_iterator_adapter(m_shapes)
		{
		}

		bc_mesh_part_collider::bc_mesh_part_collider(bc_mesh_part_collider&& p_other) noexcept
			: bc_const_iterator_adapter(m_shapes),
			m_shapes(std::move(p_other.m_shapes)),
			m_convex_shapes(std::move(p_other.m_convex_shapes)),
			m_triangle_shapes(std::move(p_other.m_triangle_shapes))
		{
		}

		bc_mesh_part_collider::~bc_mesh_part_collider()
		{
		}

		bc_mesh_part_collider& bc_mesh_part_collider::operator=(bc_mesh_part_collider&& p_other) noexcept
		{
			m_shapes = std::move(p_other.m_shapes);
			m_convex_shapes = std::move(p_other.m_convex_shapes);
			m_triangle_shapes = std::move(p_other.m_triangle_shapes);

			return *this;
		}

		void bc_mesh_part_collider::add_px_shape(const physics::bc_shape_box& p_box,
			bc_mesh_node::node_index_t p_attached_node_transform_index,
			const physics::bc_transform& p_local_transform,
			physics::bc_shape_flag p_flags)
		{
			m_shapes.push_back
			(
				bc_mesh_part_collider_entry
				(
					core::bc_make_unique< physics::bc_shape_box >(p_box),
					p_attached_node_transform_index,
					p_local_transform,
					p_flags
				)
			);
		}

		void bc_mesh_part_collider::add_px_shape(const physics::bc_shape_sphere& p_sphere,
			bc_mesh_node::node_index_t p_attached_node_transform_index,
			const physics::bc_transform& p_local_transform,
			physics::bc_shape_flag p_flags)
		{
			m_shapes.push_back
			(
				bc_mesh_part_collider_entry
				(
					core::bc_make_unique< physics::bc_shape_sphere >(p_sphere),
					p_attached_node_transform_index,
					p_local_transform,
					p_flags
				)
			);
		}

		void bc_mesh_part_collider::add_px_shape(const physics::bc_shape_capsule& p_capsule,
			bc_mesh_node::node_index_t p_attached_node_transform_index,
			const physics::bc_transform& p_local_transform,
			physics::bc_shape_flag p_flags)
		{
			m_shapes.push_back
			(
				bc_mesh_part_collider_entry
				(
					core::bc_make_unique< physics::bc_shape_capsule >(p_capsule),
					p_attached_node_transform_index,
					p_local_transform,
					p_flags
				)
			);
		}

		void bc_mesh_part_collider::add_px_shape(physics::bc_convex_mesh_ref&& p_convex,
			bc_mesh_node::node_index_t p_attached_node_transform_index,
			const physics::bc_transform& p_local_transform,
			physics::bc_shape_flag p_flags)
		{
			m_shapes.push_back
			(
				bc_mesh_part_collider_entry
				(
					core::bc_make_unique< physics::bc_shape_convex_mesh >
					(physics::bc_shape_convex_mesh(p_convex.get())),
					p_attached_node_transform_index,
					p_local_transform,
					p_flags
				)
			);
			m_convex_shapes.push_back(std::move(p_convex));
		}

		void bc_mesh_part_collider::add_px_shape(physics::bc_triangle_mesh_ref&& p_mesh,
			bc_mesh_node::node_index_t p_attached_node_transform_index,
			const physics::bc_transform& p_local_transform,
			physics::bc_shape_flag p_flags)
		{
			m_shapes.push_back
			(
				bc_mesh_part_collider_entry
				(
					core::bc_make_unique< physics::bc_shape_triangle_mesh >
					(physics::bc_shape_triangle_mesh(p_mesh.get())),
					p_attached_node_transform_index,
					p_local_transform,
					p_flags
				)
			);
			m_triangle_shapes.push_back(std::move(p_mesh));
		}

		void bc_mesh_part_collider::shrink_to_fit()
		{
			m_shapes.shrink_to_fit();
			m_convex_shapes.shrink_to_fit();
			m_triangle_shapes.shrink_to_fit();
		}
	}
}