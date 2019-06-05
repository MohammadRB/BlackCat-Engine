// [12/20/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Shape/bcShapeSphere.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_sphere< g_api_physx >::bc_platform_shape_sphere(platform_pack& p_pack)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_sphere< g_api_physx >::bc_platform_shape_sphere(bcFLOAT p_radius)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(physx::PxSphereGeometry(p_radius))
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_sphere<g_api_physx>::bc_platform_shape_sphere(const bc_platform_shape_sphere& p_other)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_sphere<g_api_physx>::~bc_platform_shape_sphere()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_sphere<g_api_physx>& bc_platform_shape_sphere<g_api_physx>::operator=(const bc_platform_shape_sphere& p_other)
		{
			bc_platform_shape_geometry::operator=(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry));
			m_pack.m_px_geometry = p_other.m_pack.m_px_geometry;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_shape_sphere<g_api_physx>::get_radius() const noexcept
		{
			return m_pack.m_px_geometry.radius;
		}
	}
}