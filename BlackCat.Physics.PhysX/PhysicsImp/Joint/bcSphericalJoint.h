// [12/15/2016 MRB]

#pragma once

#include "PhysicsImp/Joint/bcJoint.h"

#include "Physics/Joint/bcSphericalJoint.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_spherical_joint_pack<g_api_physx>
		{
		};
	}
}