// [12/20/2016 MRB]

#pragma once

#include "PhysicsImp/Shape/bcShapeGeometry.h"

#include "Physics/Shape/bcShapeSphere.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_shape_sphere_pack< g_api_physx >
		{
			explicit bc_platform_shape_sphere_pack(const physx::PxSphereGeometry& p_sphere)
				: m_px_geometry(p_sphere)
			{
			}

			physx::PxSphereGeometry m_px_geometry;
		};
	}
}