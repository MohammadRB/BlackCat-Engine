// [04/13/2017 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Fundation/bcSerializeBuffer.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_serialize_buffer< g_api_physx >::bc_platform_serialize_buffer()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_serialize_buffer< g_api_physx >::bc_platform_serialize_buffer(platform_pack& p_pack)
		{
			m_pack.m_registry = p_pack.m_registry;
			m_pack.m_collection = p_pack.m_collection;
			m_pack.m_collection_deserialize_buffer = p_pack.m_collection_deserialize_buffer;

			bcAssert(reinterpret_cast<bcUINTPTR>(m_pack.m_collection_deserialize_buffer) % 128 == 0);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_serialize_buffer< g_api_physx >::bc_platform_serialize_buffer(bc_platform_serialize_buffer&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_serialize_buffer< g_api_physx >::~bc_platform_serialize_buffer()
		{
			if(m_pack.m_collection)
			{
				m_pack.m_collection->release();
				m_pack.m_registry->release();
			}
			if(m_pack.m_collection_deserialize_buffer)
			{
				bcAlignedFree(m_pack.m_collection_deserialize_buffer);
			}
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_serialize_buffer< g_api_physx >& bc_platform_serialize_buffer< g_api_physx >::operator=(bc_platform_serialize_buffer&& p_other) noexcept
		{
			m_pack.m_registry = p_other.m_pack.m_registry;
			m_pack.m_collection = p_other.m_pack.m_collection;
			m_pack.m_collection_deserialize_buffer = p_other.m_pack.m_collection_deserialize_buffer;
			p_other.m_pack.m_registry = nullptr;
			p_other.m_pack.m_collection = nullptr;
			p_other.m_pack.m_collection_deserialize_buffer = nullptr;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_serialize_buffer< g_api_physx >::get_object_count() const noexcept
		{
			return m_pack.m_collection->getNbObjects();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void* bc_platform_serialize_buffer< g_api_physx >::get_deserialize_buffer() noexcept
		{
			void* l_result = nullptr;
			std::swap(l_result, m_pack.m_collection_deserialize_buffer);

			return l_result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_serialize_buffer< g_api_physx >::get(bcUINT32 p_index, bc_height_field& p_height_field) noexcept
		{
			physx::PxBase& l_object = m_pack.m_collection->getObject(p_index);
			physx::PxHeightField* l_height_field;

			if (!((l_height_field = l_object.is<physx::PxHeightField>())))
			{
				return false;
			}

			static_cast<bc_platform_physics_reference<g_api_physx>&>(p_height_field).get_platform_pack().m_px_object = l_height_field;

			return true;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_serialize_buffer< g_api_physx >::add(bc_height_field p_height_field) noexcept
		{
			m_pack.m_collection->add(*static_cast<bc_physics_reference&>(p_height_field).get_platform_pack().m_px_object);
		}
	}
}