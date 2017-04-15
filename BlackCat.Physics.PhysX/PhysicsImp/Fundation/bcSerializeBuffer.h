// [04/13/2017 MRB]

#pragma once

#include "PhysicsImp/Fundation/bcMemoryBuffer.h"
#include "PhysicsImp/Shape/bcHeightField.h"

#include "Physics/PhysicsPCH.h"
#include "Physics/Fundation/bcSerializeBuffer.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_serialize_buffer_pack< g_api_physx >
		{
			bc_platform_serialize_buffer_pack()
				: m_registry(nullptr),
				m_collection(nullptr),
				m_collection_deserialize_buffer(nullptr)
			{
			}

			physx::PxSerializationRegistry* m_registry;
			physx::PxCollection* m_collection;
			void* m_collection_deserialize_buffer;
		};
	}
}