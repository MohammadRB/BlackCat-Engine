// [01/21/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Physics/bcMeshPartCollider.h"

namespace black_cat
{
	namespace game
	{
		bc_mesh_part_collider::bc_mesh_part_collider()
		{
		}

		bc_mesh_part_collider::bc_mesh_part_collider(bc_mesh_part_collider&& p_other) noexcept
			: m_shapes(std::move(p_other.m_shapes)),
			m_convexs(std::move(p_other.m_convexs)),
			m_triangles(std::move(p_other.m_triangles))
		{
		}

		bc_mesh_part_collider::~bc_mesh_part_collider()
		{
		}

		bc_mesh_part_collider& bc_mesh_part_collider::operator=(bc_mesh_part_collider&& p_other) noexcept
		{
			m_shapes = std::move(p_other.m_shapes);
			m_convexs = std::move(p_other.m_convexs);
			m_triangles = std::move(p_other.m_triangles);

			return *this;
		}

		void bc_mesh_part_collider::add_px_shape(const physics::bc_shape_box& p_box, const physics::bc_transform& p_transformation)
		{
			m_shapes.push_back(bc_mesh_part_collider_entry(core::bc_make_unique<physics::bc_shape_box>(p_box), p_transformation));
		}

		void bc_mesh_part_collider::add_px_shape(const physics::bc_shape_sphere& p_sphere, const physics::bc_transform& p_transformation)
		{
			m_shapes.push_back(bc_mesh_part_collider_entry(core::bc_make_unique<physics::bc_shape_sphere>(p_sphere), p_transformation));
		}

		void bc_mesh_part_collider::add_px_shape(const physics::bc_shape_capsule& p_capsule, const physics::bc_transform& p_transformation)
		{
			m_shapes.push_back(bc_mesh_part_collider_entry(core::bc_make_unique<physics::bc_shape_capsule>(p_capsule), p_transformation));
		}

		void bc_mesh_part_collider::add_px_shape(physics::bc_convex_mesh_ref&& p_convex, const physics::bc_transform& p_transformation)
		{
			m_shapes.push_back(bc_mesh_part_collider_entry
			(
				core::bc_make_unique<physics::bc_shape_convex_mesh>(physics::bc_shape_convex_mesh(p_convex.get())),
				p_transformation
			));
			m_convexs.push_back(std::move(p_convex));
		}

		void bc_mesh_part_collider::add_px_shape(physics::bc_triangle_mesh_ref&& p_mesh, const physics::bc_transform& p_transformation)
		{
			m_shapes.push_back(bc_mesh_part_collider_entry
			(
				core::bc_make_unique<physics::bc_shape_triangle_mesh>(physics::bc_shape_triangle_mesh(p_mesh.get())), 
				p_transformation
			));
			m_triangles.push_back(std::move(p_mesh));
		}

		bc_mesh_part_collider::const_iterator bc_mesh_part_collider::cbegin() const
		{
			return m_shapes.cbegin();
		}

		bc_mesh_part_collider::const_iterator bc_mesh_part_collider::cend() const
		{
			return m_shapes.cend();
		}

		size_t bc_mesh_part_collider::size() const
		{
			return m_shapes.size();
		}

		void bc_mesh_part_collider::shrink_to_fit()
		{
			m_shapes.shrink_to_fit();
			m_convexs.shrink_to_fit();
			m_triangles.shrink_to_fit();
		}
	}
}