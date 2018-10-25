// [12/20/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Shape/bcShapeGeometry.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_geometry<g_api_physx>::~bc_platform_shape_geometry()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_geometry<g_api_physx>::bc_platform_shape_geometry(platform_pack p_pack)
			: m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_geometry< g_api_physx >::bc_platform_shape_geometry(const bc_platform_shape_geometry& p_other)
			: m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_geometry<g_api_physx>& bc_platform_shape_geometry<g_api_physx>::operator=(const bc_platform_shape_geometry& p_other)
		{
			m_pack.m_px_geometry = p_other.m_pack.m_px_geometry;

			return *this;
		}
	}
}