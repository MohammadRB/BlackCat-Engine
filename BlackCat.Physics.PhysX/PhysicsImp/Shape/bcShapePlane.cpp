// [12/20/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
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
			m_pack(physx::PxPlaneGeometry())
		{
			bcAssert(false);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_plane<g_api_physx>::bc_platform_shape_plane(const core::bc_vector3f& p_normal, const core::bc_vector3f& p_point_on_plane)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(physx::PxPlaneGeometry())
		{
			bcAssert(false);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_plane<g_api_physx>::bc_platform_shape_plane(const core::bc_vector3f& p_point0, const core::bc_vector3f& p_point1, const core::bc_vector3f& p_point2)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(physx::PxPlaneGeometry())
		{
			bcAssert(false);
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
			//bc_platform_shape_geometry::operator=(p_other);
			m_pack.m_px_geometry = p_other.m_pack.m_px_geometry;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_shape_plane<g_api_physx>::distance(const core::bc_vector3f& p_point) const noexcept
		{
			bcAssert(false);
			return 0;
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_shape_plane<g_api_physx>::get_normal() const noexcept
		{
			bcAssert(false);
			return core::bc_vector3f(0, 0, 0);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_shape_plane<g_api_physx>::get_distance() const noexcept
		{
			bcAssert(false);
			return 0;
		}
	}
}