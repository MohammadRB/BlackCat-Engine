// [15/12/2016 MRB]

#pragma once

#include "PhysicsImp/Joint/bcJoint.h"

#include "Physics/Joint/bcSphericalJoint.h"

namespace black_cat::physics
{
	template<>
	struct bc_platform_spherical_joint_pack<g_api_physx> : bc_platform_joint_pack<g_api_physx>
	{
	};
}
