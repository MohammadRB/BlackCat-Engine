// [12/16/2016 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Physics/Fundation/bcMemoryBuffer.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_memory_buffer_pack<g_api_physx>
		{
			physx::PxDefaultMemoryOutputStream* m_px_stream;
			bool m_is_valid;
		};
	}
}