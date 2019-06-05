// [12/20/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/bcUtility.h"
#include "PhysicsImp/Shape/bcShapePlane.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_plane<g_api_physx>::bc_platform_shape_plane(platform_pack& p_pack)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_plane<g_api_physx>::bc_platform_shape_plane(const core::bc_vector3f& p_normal, bcFLOAT p_distance)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(physx::PxPlaneGeometry(), physx::PxPlane(bc_to_right_hand(p_normal), p_distance))
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_plane<g_api_physx>::bc_platform_shape_plane(const core::bc_vector3f& p_normal, const core::bc_vector3f& p_point_on_plane)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(physx::PxPlaneGeometry(), physx::PxPlane(bc_to_right_hand(p_point_on_plane), bc_to_right_hand(p_normal)))
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_plane<g_api_physx>::bc_platform_shape_plane(const core::bc_vector3f& p_point0, const core::bc_vector3f& p_point1, const core::bc_vector3f& p_point2)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(physx::PxPlaneGeometry(), physx::PxPlane(bc_to_right_hand(p_point0), bc_to_right_hand(p_point1), bc_to_right_hand(p_point2)))
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_plane<g_api_physx>::bc_platform_shape_plane(const bc_platform_shape_plane& p_other)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_plane<g_api_physx>::~bc_platform_shape_plane()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_plane<g_api_physx>& bc_platform_shape_plane<g_api_physx>::operator=(const bc_platform_shape_plane& p_other)
		{
			bc_platform_shape_geometry::operator=(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry));
			m_pack = p_other.m_pack;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_shape_plane<g_api_physx>::distance(const core::bc_vector3f& p_point) const noexcept
		{
			return m_pack.m_px_plane.distance(bc_to_right_hand(p_point));
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_shape_plane<g_api_physx>::get_normal() const noexcept
		{
			return bc_to_game_hand(m_pack.m_px_plane.n);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_shape_plane<g_api_physx>::get_distance() const noexcept
		{
			return m_pack.m_px_plane.d;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_transform bc_platform_shape_plane<g_api_physx>::as_transform() const noexcept
		{
			bc_platform_transform<g_api_physx> l_transform;
			l_transform.get_platform_pack().m_px_transform = physx::PxTransformFromPlaneEquation(m_pack.m_px_plane);

			return l_transform;
		}
	}
}