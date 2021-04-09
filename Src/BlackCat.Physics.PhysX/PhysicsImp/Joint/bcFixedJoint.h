// [12/15/2016 MRB]

#pragma once

#include "PhysicsImp/Joint/bcJoint.h"

#include "Physics/Joint/bcFixedJoint.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_fixed_joint_pack<g_api_physx> : bc_platform_joint_pack<g_api_physx>
		{
		};
	}
}