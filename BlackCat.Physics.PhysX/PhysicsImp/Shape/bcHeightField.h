// [12/11/2016 MRB]

#pragma once

#include "PhysicsImp/bcPhysicsReference.h"
#include "PhysicsImp/bcPhysicsRef.h"
#include "PhysicsImp/Fundation/bcMaterial.h"

#include "Physics/Shape/bcHeightField.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_height_field_pack<g_api_physx>
		{
		};
	}
}