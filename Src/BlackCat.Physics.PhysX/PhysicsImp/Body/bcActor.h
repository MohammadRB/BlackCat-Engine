// [08/12/2016 MRB]

#pragma once

#include "PhysicsImp/bcPhysicsReference.h"
#include "PhysicsImp/bcPhysicsRef.h"
#include "PhysicsImp/Shape/bcBoundBox.h"

#include "Physics/Body/bcActor.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat::physics
{
	template<>
	struct bc_platform_actor_pack<g_api_physx> : bc_platform_physics_reference_pack<g_api_physx>
	{
	};
}
