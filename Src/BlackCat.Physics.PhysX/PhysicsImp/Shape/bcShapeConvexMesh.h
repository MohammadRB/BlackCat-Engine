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
		struct bc_platform_shape_convex_mesh_pack< g_api_physx >
		{
			explicit bc_platform_shape_convex_mesh_pack(const physx::PxConvexMeshGeometry& p_sphere)
				: m_px_geometry(p_sphere)
			{
			}

			physx::PxConvexMeshGeometry m_px_geometry;
		};
	}
}