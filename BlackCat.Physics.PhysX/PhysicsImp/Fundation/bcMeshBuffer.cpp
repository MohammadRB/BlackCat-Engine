// [12/16/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Fundation/bcMeshBuffer.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_mesh_buffer<g_api_physx>::bc_platform_mesh_buffer()
		{
			m_pack.m_px_stream = core::bc_make_unique<physx::PxDefaultMemoryOutputStream>();
			m_pack.m_is_valid = false;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_mesh_buffer<g_api_physx>::bc_platform_mesh_buffer(bc_platform_mesh_buffer&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_mesh_buffer<g_api_physx>::~bc_platform_mesh_buffer()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_mesh_buffer<g_api_physx>& bc_platform_mesh_buffer<g_api_physx>::operator=(bc_platform_mesh_buffer&& p_other) noexcept
		{
			m_pack.m_px_stream = std::move(p_other.m_pack.m_px_stream);
			m_pack.m_is_valid = p_other.m_pack.m_is_valid;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_mesh_buffer<g_api_physx>::is_valid() const noexcept
		{
			return m_pack.m_is_valid;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void* bc_platform_mesh_buffer<g_api_physx>::get_buffer_pointer() const noexcept
		{
			return m_pack.m_px_stream->getData();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_mesh_buffer<g_api_physx>::get_buffer_size() const noexcept
		{
			return m_pack.m_px_stream->getSize();
		}
	}
}