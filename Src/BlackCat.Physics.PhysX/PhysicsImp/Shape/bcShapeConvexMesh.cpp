// [20/12/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Shape/bcShapeConvexMesh.h"

namespace black_cat::physics
{
	template<>
	BC_PHYSICSIMP_DLL
	bc_platform_shape_convex_mesh<g_api_physx>::bc_platform_shape_convex_mesh() noexcept
		: bc_platform_shape_geometry()
	{
	}

	template<>
	BC_PHYSICSIMP_DLL
	bc_platform_shape_convex_mesh<g_api_physx>::bc_platform_shape_convex_mesh(platform_pack& p_pack) noexcept
		: bc_platform_shape_geometry(),
		  m_pack(p_pack)
	{
	}

	template<>
	BC_PHYSICSIMP_DLL
	bc_platform_shape_convex_mesh<g_api_physx>::bc_platform_shape_convex_mesh(const bc_convex_mesh& p_convex) noexcept
		: bc_platform_shape_geometry(),
		  m_pack(static_cast<physx::PxConvexMesh*>(p_convex.get_platform_pack().m_px_object))
	{
	}

	template<>
	BC_PHYSICSIMP_DLL
	bc_platform_shape_convex_mesh<g_api_physx>::bc_platform_shape_convex_mesh(const bc_geometry_scale& p_scale, const bc_convex_mesh& p_convex) noexcept
		: bc_platform_shape_geometry(),
		  m_pack
		  (
			  physx::PxConvexMeshGeometry
			  (
				  static_cast<physx::PxConvexMesh*>(p_convex.get_platform_pack().m_px_object),
				  physx::PxMeshScale(p_scale.m_scale)
			  )
		  )
	{
	}

	template<>
	BC_PHYSICSIMP_DLL
	bc_platform_shape_convex_mesh<g_api_physx>::bc_platform_shape_convex_mesh(const bc_platform_shape_convex_mesh& p_other) noexcept
		: bc_platform_shape_geometry(p_other),
		  m_pack(p_other.m_pack)
	{
	}

	template<>
	BC_PHYSICSIMP_DLL
	bc_platform_shape_convex_mesh<g_api_physx>::~bc_platform_shape_convex_mesh()
	{
	}

	template<>
	BC_PHYSICSIMP_DLL
	bc_platform_shape_convex_mesh<g_api_physx>& bc_platform_shape_convex_mesh<g_api_physx>::operator=(const bc_platform_shape_convex_mesh& p_other) noexcept
	{
		bc_platform_shape_geometry::operator=(p_other);
		m_pack.m_px_convex = p_other.m_pack.m_px_convex;

		return *this;
	}

	template<>
	BC_PHYSICSIMP_DLL
	bc_geometry_scale bc_platform_shape_convex_mesh<g_api_physx>::get_scale() const noexcept
	{
		return bc_geometry_scale(m_pack.m_px_convex.scale.scale.x);
	}

	template<>
	BC_PHYSICSIMP_DLL
	bc_convex_mesh bc_platform_shape_convex_mesh<g_api_physx>::get_convex() const noexcept
	{
		bc_convex_mesh l_result;
		static_cast<bc_physics_reference&>(l_result).get_platform_pack().m_px_object = m_pack.m_px_convex.convexMesh;

		return l_result;
	}
}
