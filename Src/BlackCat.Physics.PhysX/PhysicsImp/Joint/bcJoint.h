// [15/12/2016 MRB]

#pragma once

#include "PhysicsImp/Fundation/bcTransform.h"
#include "PhysicsImp/bcPhysicsReference.h"
#include "PhysicsImp/bcPhysicsRef.h"

#include "Physics/Joint/bcJoint.h"

namespace black_cat::physics
{
	template<>
	struct bc_platform_joint_pack<g_api_physx> : bc_platform_physics_reference_pack<g_api_physx>
	{
	};
}
