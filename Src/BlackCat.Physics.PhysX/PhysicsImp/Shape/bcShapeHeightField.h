// [12/20/2016 MRB]

#pragma once

#include "PhysicsImp/Shape/bcHeightField.h"
#include "PhysicsImp/Shape/bcShapeGeometry.h"

#include "Physics/Shape/bcShapeHeightField.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_shape_height_field_pack< g_api_physx >
		{
			explicit bc_platform_shape_height_field_pack(const physx::PxHeightFieldGeometry& p_height_field)
				: m_px_geometry(p_height_field)
			{
			}

			physx::PxHeightFieldGeometry m_px_geometry;
		};
	}
}