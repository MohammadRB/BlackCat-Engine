// [12/20/2016 MRB]

#pragma once

#include "PhysicsImp/Shape/bcShapeGeometry.h"

#include "Physics/Shape/bcShapeBox.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_shape_box_pack< g_api_physx >
		{
			explicit bc_platform_shape_box_pack(const physx::PxBoxGeometry& p_sphere)
				: m_px_geometry(p_sphere)
			{
			}

			physx::PxBoxGeometry m_px_geometry;
		};
	}
}