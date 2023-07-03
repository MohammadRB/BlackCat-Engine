// [11/12/2016 MRB]

#pragma once

#include "PhysicsImp/Fundation/bcMaterial.h"
#include "PhysicsImp/Shape/bcHeightFieldSampleArray.h"
#include "PhysicsImp/bcPhysicsReference.h"
#include "PhysicsImp/bcPhysicsRef.h"

#include "Physics/Shape/bcHeightField.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_height_field_pack<g_api_physx> : bc_platform_physics_reference_pack<g_api_physx>
		{
		};
	}
}