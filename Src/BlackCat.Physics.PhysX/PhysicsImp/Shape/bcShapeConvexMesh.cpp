// [12/20/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Shape/bcShapeConvexMesh.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_convex_mesh< g_api_physx >::bc_platform_shape_convex_mesh(platform_pack& p_pack)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_convex_mesh< g_api_physx >::bc_platform_shape_convex_mesh(const bc_convex_mesh& p_convex)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(physx::PxConvexMeshGeometry())
		{
			m_pack.m_px_geometry.convexMesh = static_cast< physx::PxConvexMesh* >
			(
				static_cast< bc_physics_reference& >(const_cast< bc_convex_mesh& >(p_convex)).get_platform_pack().m_px_object
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_convex_mesh< g_api_physx >::bc_platform_shape_convex_mesh(const bc_geometry_scale& p_scale, const bc_convex_mesh& p_convex)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(physx::PxConvexMeshGeometry())
		{
			m_pack.m_px_geometry.convexMesh = static_cast< physx::PxConvexMesh* >
			(
				static_cast< bc_physics_reference& >(const_cast< bc_convex_mesh& >(p_convex)).get_platform_pack().m_px_object
			);
			m_pack.m_px_geometry.scale = physx::PxMeshScale(p_scale.m_scale);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_convex_mesh< g_api_physx >::bc_platform_shape_convex_mesh(const bc_platform_shape_convex_mesh& p_other)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_convex_mesh< g_api_physx >::~bc_platform_shape_convex_mesh()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_convex_mesh< g_api_physx >& bc_platform_shape_convex_mesh< g_api_physx >::operator=(const bc_platform_shape_convex_mesh& p_other)
		{
			bc_platform_shape_geometry::operator=(p_other);
			m_pack.m_px_geometry = p_other.m_pack.m_px_geometry;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_geometry_scale bc_platform_shape_convex_mesh< g_api_physx >::get_scale() const noexcept
		{
			return bc_geometry_scale(m_pack.m_px_geometry.scale.scale.x);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_convex_mesh bc_platform_shape_convex_mesh< g_api_physx >::get_convex() const noexcept
		{
			bc_convex_mesh l_result;
			static_cast< bc_physics_reference& >(l_result).get_platform_pack().m_px_object = m_pack.m_px_geometry.convexMesh;

			return l_result;
		}
	}
}
