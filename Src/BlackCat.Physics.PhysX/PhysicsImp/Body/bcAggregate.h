// [12/10/2016 MRB]

#pragma once

#include "PhysicsImp/bcPhysicsReference.h"
#include "PhysicsImp/bcPhysicsRef.h"

#include "Physics/Body/bcAggregate.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template< >
		struct bc_platform_aggregate_pack< g_api_physx >
		{
			constexpr static bcUINT32 s_max_actor_count = 128;
		};
	}
}