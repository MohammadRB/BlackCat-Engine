// [01/11/2019 MRB]

#include "Game/GamePCH.h"

#include "PhysicsImp/Collision/bcShapeQuery.h"
#include "PhysicsImp/Shape/bcShapeBox.h"
#include "Game/System/Input/bcCameraFrustum.h"

namespace black_cat
{
	namespace game
	{
		bc_camera_frustum::bc_camera_frustum(const bc_icamera& p_camera)
		{
			bc_icamera::extend l_camera_extends;
			p_camera.get_extend_points(l_camera_extends);

			m_planes.reserve(6);

			// Near
			m_planes.push_back(physics::bc_shape_plane(l_camera_extends[2], l_camera_extends[1], l_camera_extends[3]));
			// Far
			m_planes.push_back(physics::bc_shape_plane(l_camera_extends[5], l_camera_extends[6], l_camera_extends[4]));
			// Left
			m_planes.push_back(physics::bc_shape_plane(l_camera_extends[1], l_camera_extends[5], l_camera_extends[0]));
			// Top
			m_planes.push_back(physics::bc_shape_plane(l_camera_extends[1], l_camera_extends[2], l_camera_extends[5]));
			// Right
			m_planes.push_back(physics::bc_shape_plane(l_camera_extends[2], l_camera_extends[3], l_camera_extends[6]));
			// Bottom
			m_planes.push_back(physics::bc_shape_plane(l_camera_extends[0], l_camera_extends[4], l_camera_extends[3]));
		}

		bc_camera_frustum::bc_camera_frustum(bc_camera_frustum&& p_other) noexcept
			: m_planes(std::move(p_other.m_planes))
		{
		}

		bc_camera_frustum::~bc_camera_frustum()
		{
		}

		bc_camera_frustum& bc_camera_frustum::operator=(bc_camera_frustum&& p_other) noexcept
		{
			m_planes = std::move(p_other.m_planes);
			return *this;
		}

		bool bc_camera_frustum::intersects(const physics::bc_bound_box& p_box) const noexcept
		{
			const physics::bc_shape_box l_box(p_box.get_half_extends());
			const physics::bc_transform l_box_transform(p_box.get_center());

			for (auto& l_plane : m_planes)
			{
				const bool l_intersects = physics::bc_shape_query::overlap(l_box, l_box_transform, l_plane, l_plane.as_transform());
				if(!l_intersects)
				{
					return false;
				}
			}

			return true;
		}
	}
}