// [12/16/2016 MRB]

#pragma once

#include "PhysicsImp/Fundation/bcTransform.h"
#include "PhysicsImp/Shape/bcBoundBox.h"

#include "Physics/Collision/bcShapeQuery.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_ray_hit_pack<g_api_physx>
		{
			physx::PxRaycastHit m_px_hit;
		};

		template<>
		struct bc_platform_overlap_hit_pack<g_api_physx>
		{
			physx::PxOverlapHit m_px_hit;
		};

		template<>
		struct bc_platform_sweep_hit_pack<g_api_physx>
		{
			physx::PxSweepHit m_px_hit;
		};
	}
}