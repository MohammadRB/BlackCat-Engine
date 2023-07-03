// [16/12/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Fundation/bcMemoryBuffer.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_memory_buffer<g_api_physx>::bc_platform_memory_buffer()
		{
			m_pack.m_px_stream = BC_NEW(physx::PxDefaultMemoryOutputStream(), core::bc_alloc_type::unknown);
			m_pack.m_is_valid = false;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_memory_buffer<g_api_physx>::bc_platform_memory_buffer(platform_pack&& p_pack)
			: m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_memory_buffer<g_api_physx>::bc_platform_memory_buffer(bc_platform_memory_buffer&& p_other) noexcept
			: m_pack(p_other.m_pack)
		{
			p_other.m_pack.m_px_stream = nullptr;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_memory_buffer<g_api_physx>::~bc_platform_memory_buffer()
		{
			if(m_pack.m_px_stream)
			{
				// because of an unknown error with dynamic_cast<void*>() in bcAlloc.h we have used free routine instead of delete
				m_pack.m_px_stream->~PxDefaultMemoryOutputStream();
				BC_FREE(m_pack.m_px_stream);
			}
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_memory_buffer<g_api_physx>& bc_platform_memory_buffer<g_api_physx>::operator=(bc_platform_memory_buffer&& p_other) noexcept
		{
			m_pack = p_other.m_pack;
			p_other.m_pack.m_px_stream = nullptr;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_memory_buffer<g_api_physx>::is_valid() const noexcept
		{
			return m_pack.m_is_valid;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void* bc_platform_memory_buffer<g_api_physx>::get_buffer_pointer() const noexcept
		{
			return m_pack.m_px_stream->getData();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_memory_buffer<g_api_physx>::get_buffer_size() const noexcept
		{
			return m_pack.m_px_stream->getSize();
		}
	}
}