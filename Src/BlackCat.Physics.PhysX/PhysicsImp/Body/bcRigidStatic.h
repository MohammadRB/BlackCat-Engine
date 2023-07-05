// [15/12/2016 MRB]

#pragma once

#include "PhysicsImp/Body/bcRigidBody.h"

#include "Physics/Body/bcRigidStatic.h"

namespace black_cat::physics
{
	template<>
	struct bc_platform_rigid_static_pack<g_api_physx> : bc_platform_rigid_body_pack<g_api_physx>
	{
	};
}
