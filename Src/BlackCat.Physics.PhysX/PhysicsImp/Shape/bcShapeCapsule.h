// [12/20/2016 MRB]

#pragma once

#include "PhysicsImp/Shape/bcShapeGeometry.h"

#include "Physics/Shape/bcShapeCapsule.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_shape_capsule_pack< g_api_physx >
		{
			explicit bc_platform_shape_capsule_pack(const physx::PxCapsuleGeometry& p_capsule)
				: m_px_geometry(p_capsule)
			{
			}

			physx::PxCapsuleGeometry m_px_geometry;
		};
	}
}