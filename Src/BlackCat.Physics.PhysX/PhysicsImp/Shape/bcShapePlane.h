// [12/20/2016 MRB]

#pragma once

#include "PhysicsImp/Shape/bcShapeGeometry.h"
#include "Physicsimp/Fundation/bcTransform.h"

#include "Physics/Shape/bcShapePlane.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_shape_plane_pack< g_api_physx >
		{
			explicit bc_platform_shape_plane_pack(const physx::PxPlaneGeometry& p_sphere, const physx::PxPlane& p_plane)
				: m_px_geometry(p_sphere),
				m_px_plane(p_plane)
			{
			}

			physx::PxPlaneGeometry m_px_geometry;
			physx::PxPlane m_px_plane;
		};
	}
}