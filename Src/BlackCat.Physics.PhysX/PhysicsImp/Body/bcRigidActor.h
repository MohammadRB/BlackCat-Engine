// [12/14/2016 MRB]

#pragma once

#include "PhysicsImp/Fundation/bcTransform.h"
#include "PhysicsImp/Shape/bcShape.h"
#include "PhysicsImp/Body/bcActor.h"

#include "Physics/Body/bcRigidActor.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_rigid_actor_pack<g_api_physx> : bc_platform_actor_pack<g_api_physx>
		{
		};
	}
}