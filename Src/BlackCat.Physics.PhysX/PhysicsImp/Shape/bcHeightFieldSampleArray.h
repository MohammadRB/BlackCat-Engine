// [02/14/2021 MRB]

#pragma once

#include "PhysicsImp/Fundation/bcMaterial.h"

#include "Core/Memory/bcPtr.h"
#include "Physics/Shape/bcHeightFieldSampleArray.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_height_field_sample_array_pack< g_api_physx >
		{
			bcUINT32 m_num_rows;
			bcUINT32 m_num_columns;
			core::bc_unique_ptr<physx::PxHeightFieldSample> m_buffer;
		};
	}
}