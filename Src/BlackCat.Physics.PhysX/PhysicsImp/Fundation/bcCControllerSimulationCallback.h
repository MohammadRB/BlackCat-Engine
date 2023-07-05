// [22/03/2021 MRB]

#pragma once

#include "PhysicsImp/Shape/bcShape.h"
#include "PhysicsImp/Body/bcRigidActor.h"

#include "Physics/Fundation/bcCControllerSimulationCallback.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat::physics
{
	template<>
	struct bc_platform_ccontroller_shape_hit_pack<g_api_physx>
	{
		const physx::PxControllerShapeHit* m_px_hit;
	};

	template<>
	struct bc_platform_ccontroller_controller_hit_pack<g_api_physx>
	{
		const physx::PxControllersHit* m_px_hit;
	};
}
