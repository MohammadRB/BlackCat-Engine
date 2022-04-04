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
		bc_platform_shape_sphere<g_api_physx>::bc_platform_shape_sphere() noexcept
			: bc_platform_shape_geometry()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_sphere< g_api_physx >::bc_platform_shape_sphere(platform_pack& p_pack) noexcept
			: bc_platform_shape_geometry(),
			m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_sphere< g_api_physx >::bc_platform_shape_sphere(bcFLOAT p_radius) noexcept
			: bc_platform_shape_geometry(),
			m_pack(physx::PxSphereGeometry(p_radius))
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_sphere<g_api_physx>::bc_platform_shape_sphere(const bc_platform_shape_sphere& p_other) noexcept
			: bc_platform_shape_geometry(p_other),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_sphere<g_api_physx>::~bc_platform_shape_sphere() noexcept
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_sphere<g_api_physx>& bc_platform_shape_sphere<g_api_physx>::operator=(const bc_platform_shape_sphere& p_other) noexcept
		{
			bc_platform_shape_geometry::operator=(p_other);
			m_pack = p_other.m_pack;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_shape_sphere<g_api_physx>::get_radius() const noexcept
		{
			return m_pack.m_px_sphere.radius;
		}
	}
}