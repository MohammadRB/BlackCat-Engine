// [16/12/2016 MRB]

#pragma once

#include "Physics/Fundation/bcTask.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat::physics
{
	template<>
	struct bc_platform_task_pack<g_api_physx>
	{
		explicit bc_platform_task_pack(physx::PxBaseTask* p_px_task)
			: m_px_task(p_px_task)
		{
		}

		physx::PxBaseTask* m_px_task;
	};
}
