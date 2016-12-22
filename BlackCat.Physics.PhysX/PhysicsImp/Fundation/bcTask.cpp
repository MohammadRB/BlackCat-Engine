// [12/16/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Fundation/bcTask.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_task<g_api_physx>::bc_platform_task()
		{
			m_pack.m_px_task = nullptr;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_task<g_api_physx>::bc_platform_task(bc_platform_task&& p_other) noexcept
		{
			m_pack.m_px_task = p_other.m_pack.m_px_task;
			p_other.m_pack.m_px_task = nullptr;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_task<g_api_physx>::~bc_platform_task()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_task<g_api_physx>& bc_platform_task<g_api_physx>::operator=(bc_platform_task&& p_other) noexcept
		{
			m_pack.m_px_task = p_other.m_pack.m_px_task;
			p_other.m_pack.m_px_task = nullptr;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_task<g_api_physx>::run()
		{
#ifdef BC_DEBUG
			m_pack.m_px_task->runProfiled();
#else
			m_pack.m_px_task->run();
#endif
			m_pack.m_px_task->release();
		}
	}
}