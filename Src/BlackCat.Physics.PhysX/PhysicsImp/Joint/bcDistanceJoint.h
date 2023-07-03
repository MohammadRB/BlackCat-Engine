// [15/12/2016 MRB]

#pragma once

#include "PhysicsImp/Joint/bcJoint.h"

#include "Physics/Joint/bcDistanceJoint.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_distance_joint_pack<g_api_physx> : bc_platform_joint_pack<g_api_physx>
		{
		};
	}
}