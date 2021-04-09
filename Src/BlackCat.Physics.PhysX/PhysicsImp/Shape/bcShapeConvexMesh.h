// [12/20/2016 MRB]

#pragma once

#include "PhysicsImp/Shape/bcConvexMesh.h"
#include "PhysicsImp/Shape/bcShapeGeometry.h"

#include "Physics/Shape/bcShapeConvexMesh.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_shape_convex_mesh_pack<g_api_physx> : bc_platform_shape_geometry_pack<g_api_physx>
		{
			explicit bc_platform_shape_convex_mesh_pack(const physx::PxConvexMeshGeometry& p_px_convex)
				: bc_platform_shape_geometry_pack(m_px_convex),
				m_px_convex(p_px_convex)
			{
			}

			bc_platform_shape_convex_mesh_pack(const bc_platform_shape_convex_mesh_pack& p_other)
				: bc_platform_shape_geometry_pack(m_px_convex),
				m_px_convex(p_other.m_px_convex)
			{
			}

			~bc_platform_shape_convex_mesh_pack() = default;

			bc_platform_shape_convex_mesh_pack& operator=(const bc_platform_shape_convex_mesh_pack& p_other)
			{
				bc_platform_shape_geometry_pack::operator=(m_px_convex);
				m_px_convex = p_other.m_px_convex;

				return *this;
			}

			physx::PxConvexMeshGeometry m_px_convex;
		};
	}
}