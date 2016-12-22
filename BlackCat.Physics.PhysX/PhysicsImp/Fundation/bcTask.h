// [12/16/2016 MRB]

#pragma once

#include "Physics/Fundation/bcTask.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_task_pack<g_api_physx>
		{
			physx::PxBaseTask* m_px_task;
		};
	}
}