// [12/20/2016 MRB]

#pragma once

#include "PhysicsImp/Shape/bcTriangleMesh.h"
#include "PhysicsImp/Shape/bcShapeGeometry.h"

#include "Physics/Shape/bcShapeTriangleMesh.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_shape_triangle_mesh_pack< g_api_physx >
		{
			explicit bc_platform_shape_triangle_mesh_pack(const physx::PxTriangleMeshGeometry& p_sphere)
				: m_px_geometry(p_sphere)
			{
			}

			physx::PxTriangleMeshGeometry m_px_geometry;
		};
	}
}