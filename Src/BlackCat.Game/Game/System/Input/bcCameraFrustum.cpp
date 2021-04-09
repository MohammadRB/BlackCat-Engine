// [01/11/2019 MRB]

#include "Game/GamePCH.h"

#include "PhysicsImp/Collision/bcShapeQuery.h"
#include "PhysicsImp/Shape/bcShapeBox.h"
#include "Game/System/Input/bcCameraFrustum.h"

namespace black_cat
{
	namespace game
	{
		bc_camera_frustum::bc_camera_frustum(const bci_camera& p_camera) noexcept
			: m_planes
			({ 
				physics::bc_shape_plane(core::bc_vector3f(0), 0),
				physics::bc_shape_plane(core::bc_vector3f(0), 0),
				physics::bc_shape_plane(core::bc_vector3f(0), 0),
				physics::bc_shape_plane(core::bc_vector3f(0), 0),
				physics::bc_shape_plane(core::bc_vector3f(0), 0),
				physics::bc_shape_plane(core::bc_vector3f(0), 0)
			})
		{
			bci_camera::extend l_camera_extends;
			p_camera.get_extend_points(l_camera_extends);

			_construct(l_camera_extends);
		}

		bc_camera_frustum::bc_camera_frustum(const bc_camera_instance& p_camera) noexcept
			: m_planes
			({
				physics::bc_shape_plane(core::bc_vector3f(0), 0),
				physics::bc_shape_plane(core::bc_vector3f(0), 0),
				physics::bc_shape_plane(core::bc_vector3f(0), 0),
				physics::bc_shape_plane(core::bc_vector3f(0), 0),
				physics::bc_shape_plane(core::bc_vector3f(0), 0),
				physics::bc_shape_plane(core::bc_vector3f(0), 0)
			})
		{
			_construct(p_camera.get_extends());
		}

		bc_camera_frustum::bc_camera_frustum(const bc_camera_frustum& p_other) noexcept = default;

		bc_camera_frustum::~bc_camera_frustum() = default;

		bc_camera_frustum& bc_camera_frustum::operator=(const bc_camera_frustum& p_other) noexcept = default;

		bool bc_camera_frustum::intersects(const physics::bc_bound_box& p_box) const noexcept
		{
			const physics::bc_shape_box l_box(p_box.get_half_extends());
			const physics::bc_transform l_box_transform(p_box.get_center());

			for (const auto& l_plane : m_planes)
			{
				const bool l_intersects = physics::bc_shape_query::overlap(l_box, l_box_transform, l_plane, l_plane.as_transform());
				if(!l_intersects)
				{
					return false;
				}
			}

			return true;
		}

		void bc_camera_frustum::_construct(const bci_camera::extend& p_extends)
		{
			// Near
			m_planes[0] = physics::bc_shape_plane(p_extends[2], p_extends[1], p_extends[3]);
			// Far
			m_planes[1] = physics::bc_shape_plane(p_extends[5], p_extends[6], p_extends[4]);
			// Left
			m_planes[2] = physics::bc_shape_plane(p_extends[1], p_extends[5], p_extends[0]);
			// Top
			m_planes[3] = physics::bc_shape_plane(p_extends[1], p_extends[2], p_extends[5]);
			// Right
			m_planes[4] = physics::bc_shape_plane(p_extends[2], p_extends[3], p_extends[6]);
			// Bottom
			m_planes[5] = physics::bc_shape_plane(p_extends[0], p_extends[4], p_extends[3]);
		}
	}
}