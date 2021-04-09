#pragma once

#include "PhysicsImp/Joint/bcJoint.h"

#include "Physics/Joint/bcD6Joint.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_d6_joint_pack<g_api_physx> : bc_platform_joint_pack<g_api_physx>
		{
		};
	}
}