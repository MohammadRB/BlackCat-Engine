// [02/09/2017 MRB]

#pragma once

#include "Core/Container/bcIteratorAdapter.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcSpan.h"
#include "Core/Content/bcContent.h"
#include "PhysicsImp/Fundation/bcTransform.h"
#include "PhysicsImp/Shape/bcShape.h"
#include "Game/Object/Mesh/bcMeshNode.h"
#include "Game/Object/Mesh/bcSkinnedMeshCollider.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		struct BC_GAME_DLL bc_mesh_part_collider_entry
		{
			bc_mesh_part_collider_entry(bc_mesh_node::node_index_t p_attached_node_index,
				core::bc_string p_attached_mesh_name,
				core::bc_unique_ptr<physics::bc_shape_geometry> p_px_shape,
				const physics::bc_transform& p_local_transform)
				: bc_mesh_part_collider_entry
				(
					p_attached_node_index,
					p_attached_mesh_name,
					std::move(p_px_shape),
					p_local_transform,
					physics::bc_shape_flag::default_v
				)
			{
			}

			bc_mesh_part_collider_entry(bc_mesh_node::node_index_t p_attached_node_transform_index,
				core::bc_string p_attached_mesh_name,
				core::bc_unique_ptr<physics::bc_shape_geometry> p_px_shape,
				const physics::bc_transform& p_local_transform,
				physics::bc_shape_flag p_flag)
				: bc_mesh_part_collider_entry
				(
					p_attached_node_transform_index,
					p_attached_mesh_name,
					std::move(p_px_shape),
					p_local_transform,
					p_flag,
					false
				)
			{
			}

			bc_mesh_part_collider_entry(bc_mesh_node::node_index_t p_attached_node_transform_index,
				core::bc_string p_attached_mesh_name,
				core::bc_unique_ptr<physics::bc_shape_geometry> p_px_shape,
				const physics::bc_transform& p_local_transform,
				physics::bc_shape_flag p_flag,
				bool p_high_detail_query_shape)
				: m_attached_node_index(p_attached_node_transform_index),
				m_attached_mesh_name(std::move(p_attached_mesh_name)),
				m_shape(std::move(p_px_shape)),
				m_shape_flags(p_flag),
				m_local_transform(p_local_transform),
				m_initial_transform(),
				m_high_detail_query_shape(p_high_detail_query_shape)
			{

			}

			bc_mesh_part_collider_entry(bc_mesh_part_collider_entry&&) = default;

			~bc_mesh_part_collider_entry() = default;

			bc_mesh_part_collider_entry& operator=(bc_mesh_part_collider_entry&&) = default;

			bc_mesh_node::node_index_t m_attached_node_index;
			core::bc_string m_attached_mesh_name;
			core::bc_unique_ptr<physics::bc_shape_geometry> m_shape;
			physics::bc_shape_flag m_shape_flags;
			physics::bc_transform m_local_transform;
			physics::bc_transform m_initial_transform;
			bool m_high_detail_query_shape;
		};
		
		class BC_GAME_DLL bc_mesh_collider : public core::bci_content, public core::bc_const_iterator_adapter<core::bc_vector<bc_mesh_part_collider_entry>>
		{
			BC_CONTENT(msh_cld)
			friend class bc_mesh_collider_builder;

		public:
			bc_mesh_collider();

			bc_mesh_collider(bc_mesh_collider&& p_other) noexcept;

			~bc_mesh_collider() override = default;

			bc_mesh_collider& operator=(bc_mesh_collider&& p_other) noexcept;

			bc_skinned_mesh_collider& get_skinned_collider() noexcept;
			
			const bc_skinned_mesh_collider& get_skinned_collider() const noexcept;
			
			core::bc_const_span<bc_mesh_part_collider_entry> find_mesh_collider(std::string_view p_mesh_name) const noexcept;

		private:
			core::bc_vector<std::tuple<core::bc_string, bcSIZE, bcSIZE>> m_mesh_colliders;
			core::bc_vector<bc_mesh_part_collider_entry> m_colliders;
			core::bc_vector<physics::bc_convex_mesh_ref> m_convex_shapes;
			core::bc_vector<physics::bc_triangle_mesh_ref> m_triangle_shapes;
			bc_skinned_mesh_collider m_skinned_collider;
		};

		using bc_mesh_collider_ptr = core::bc_content_ptr<bc_mesh_collider>;

		inline bc_skinned_mesh_collider& bc_mesh_collider::get_skinned_collider() noexcept
		{
			return m_skinned_collider;
		}

		inline const bc_skinned_mesh_collider& bc_mesh_collider::get_skinned_collider() const noexcept
		{
			return m_skinned_collider;
		}
	}
}