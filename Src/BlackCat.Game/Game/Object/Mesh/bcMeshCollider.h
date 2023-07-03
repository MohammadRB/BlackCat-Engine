// [09/02/2017 MRB]

#pragma once

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
			bc_mesh_part_collider_entry(core::bc_string p_name,
				core::bc_string p_attached_node_name,
				bc_mesh_node::node_index_t p_attached_node_index,
				core::bc_unique_ptr<physics::bc_shape_geometry> p_px_shape,
				const physics::bc_transform& p_local_transform)
				: bc_mesh_part_collider_entry
				(
					std::move(p_name),
					std::move(p_attached_node_name),
					p_attached_node_index,
					std::move(p_px_shape),
					p_local_transform,
					physics::bc_shape_flag::default_v
				)
			{
			}

			bc_mesh_part_collider_entry(core::bc_string p_name,
				core::bc_string p_attached_node_name,
				bc_mesh_node::node_index_t p_attached_node_transform_index,
				core::bc_unique_ptr<physics::bc_shape_geometry> p_px_shape,
				const physics::bc_transform& p_local_transform,
				physics::bc_shape_flag p_flag)
				: bc_mesh_part_collider_entry
				(
					std::move(p_name),
					std::move(p_attached_node_name),
					p_attached_node_transform_index,
					std::move(p_px_shape),
					p_local_transform,
					p_flag,
					false
				)
			{
			}

			bc_mesh_part_collider_entry(core::bc_string p_name,
				core::bc_string p_attached_node_name,
				bc_mesh_node::node_index_t p_attached_node_transform_index,
				core::bc_unique_ptr<physics::bc_shape_geometry> p_px_shape,
				const physics::bc_transform& p_local_transform,
				physics::bc_shape_flag p_flag,
				bool p_high_detail_query_shape)
				: m_name(std::move(p_name)),
				m_attached_node_name(std::move(p_attached_node_name)),
				m_attached_node_index(p_attached_node_transform_index),
				m_shape(std::move(p_px_shape)),
				m_shape_flags(p_flag),
				m_local_transform(p_local_transform),
				m_absolute_transform(),
				m_high_detail_query_shape(p_high_detail_query_shape)
			{
			}

			bc_mesh_part_collider_entry(bc_mesh_part_collider_entry&&) = default;

			~bc_mesh_part_collider_entry() = default;

			bc_mesh_part_collider_entry& operator=(bc_mesh_part_collider_entry&&) = default;

			core::bc_string m_name;
			core::bc_string m_attached_node_name;
			bc_mesh_node::node_index_t m_attached_node_index;
			core::bc_unique_ptr<physics::bc_shape_geometry> m_shape;
			physics::bc_shape_flag m_shape_flags;
			physics::bc_transform m_local_transform;
			physics::bc_transform m_absolute_transform;
			bool m_high_detail_query_shape;
		};

		struct bc_mesh_part_collider_joint_entry
		{
			using hash_t = std::hash<core::bc_string_view>;

			bcSIZE m_collider1;
			bcSIZE m_collider2;
			physics::bc_transform m_transform;
		};
		
		class BC_GAME_DLL bc_mesh_collider : public core::bci_content
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

			bc_mesh_part_collider_entry* find_collider_by_name(core::bc_string_view p_name) const noexcept;

			core::bc_const_span<bc_mesh_part_collider_entry> find_mesh_node_colliders(core::bc_string_view p_node_name) const noexcept;

			const physics::bc_transform* find_joint(core::bc_string_view p_collider1, core::bc_string_view p_collider2) const noexcept;

			core::bc_span<bc_mesh_part_collider_entry> get_colliders() noexcept;
			
			core::bc_const_span<bc_mesh_part_collider_entry> get_colliders() const noexcept;

			core::bc_span<bc_mesh_part_collider_joint_entry> get_collider_joints() noexcept;

			core::bc_const_span<bc_mesh_part_collider_joint_entry> get_collider_joints() const noexcept;
			
		private:
			core::bc_vector<std::tuple<core::bc_string, bcSIZE, bcSIZE>> m_mesh_colliders;
			core::bc_vector<bc_mesh_part_collider_entry> m_colliders;
			core::bc_vector<physics::bc_convex_mesh_ref> m_convex_shapes;
			core::bc_vector<physics::bc_triangle_mesh_ref> m_triangle_shapes;
			core::bc_vector<bc_mesh_part_collider_joint_entry> m_joints;
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

		inline core::bc_span<bc_mesh_part_collider_entry> bc_mesh_collider::get_colliders() noexcept
		{
			return core::bc_make_span(m_colliders);
		}

		inline core::bc_const_span<bc_mesh_part_collider_entry> bc_mesh_collider::get_colliders() const noexcept
		{
			return core::bc_make_span(m_colliders);
		}

		inline core::bc_span<bc_mesh_part_collider_joint_entry> bc_mesh_collider::get_collider_joints() noexcept
		{
			return core::bc_make_span(m_joints);
		}

		inline core::bc_const_span<bc_mesh_part_collider_joint_entry> bc_mesh_collider::get_collider_joints() const noexcept
		{
			return core::bc_make_span(m_joints);
		}
	}
}