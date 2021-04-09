// [12/15/2016 MRB]

#pragma once

#include "PhysicsImp/Fundation/bcTransform.h"
#include "PhysicsImp/Body/bcRigidBody.h"

#include "Physics/Body/bcRigidDynamic.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_rigid_dynamic_pack<g_api_physx> : bc_platform_rigid_body_pack<g_api_physx>
		{
		};
	}
}