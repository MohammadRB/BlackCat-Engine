// [13/12/2016 MRB]

#pragma once

#include "Physics/Fundation/bcTransform.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat::physics
{
	template<>
	struct bc_platform_transform_pack<g_api_physx>
	{
		physx::PxTransform m_px_transform;
	};
}
