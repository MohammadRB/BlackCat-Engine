// [16/12/2016 MRB]

#pragma once

#include "Physics/Fundation/bcSceneDebug.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat::physics
{
	template<>
	struct bc_platform_scene_debug_pack<g_api_physx>
	{
		const physx::PxRenderBuffer* m_px_debug;
	};
}
