// [12/20/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Shape/bcShapeCapsule.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_capsule<g_api_physx>::bc_platform_shape_capsule(platform_pack& p_pack)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_capsule<g_api_physx>::bc_platform_shape_capsule(bcFLOAT p_half_height, bcFLOAT p_radius)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(physx::PxCapsuleGeometry(p_radius, p_half_height))
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_capsule<g_api_physx>::bc_platform_shape_capsule(const bc_platform_shape_capsule& p_other)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_capsule<g_api_physx>::~bc_platform_shape_capsule()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_capsule<g_api_physx>& bc_platform_shape_capsule<g_api_physx>::operator=(const bc_platform_shape_capsule& p_other)
		{
			bc_platform_shape_geometry::operator=(p_other);
			m_pack.m_px_geometry = p_other.m_pack.m_px_geometry;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_shape_capsule<g_api_physx>::get_half_height() const noexcept
		{
			return m_pack.m_px_geometry.halfHeight;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_shape_capsule<g_api_physx>::get_radius() const noexcept
		{
			return m_pack.m_px_geometry.radius;
		}
	}
}