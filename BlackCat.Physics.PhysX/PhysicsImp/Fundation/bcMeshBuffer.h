// [12/16/2016 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Physics/Fundation/bcMeshBuffer.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_mesh_buffer_pack<g_api_physx>
		{
			core::bc_unique_ptr< physx::PxDefaultMemoryOutputStream > m_px_stream;
			bool m_is_valid;
		};
	}
}