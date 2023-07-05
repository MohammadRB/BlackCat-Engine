// [08/12/2016 MRB]

#pragma once

#include "PhysicsImp/Fundation/bcTransform.h"

#include "Physics/Shape/bcBoundBox.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat::physics
{
	template<>
	struct bc_platform_bound_box_pack<g_api_physx>
	{
	public:
		physx::PxBounds3 m_bound;
	};
}
