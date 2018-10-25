// [12/15/2016 MRB]

#pragma once

#include "PhysicsImp/Body/bcRigidActor.h"

#include "Physics/Body/bcRigidBody.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_rigid_body_pack<g_api_physx>
		{
		};
	}
}