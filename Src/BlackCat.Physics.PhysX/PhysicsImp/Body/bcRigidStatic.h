// [12/15/2016 MRB]

#pragma once

#include "PhysicsImp/Body/bcRigidBody.h"

#include "Physics/Body/bcRigidStatic.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_rigid_static_pack< g_api_physx >
		{
		};
	}
}