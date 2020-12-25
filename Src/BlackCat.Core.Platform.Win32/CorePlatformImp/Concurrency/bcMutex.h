// [10/11/2014 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "CorePlatform/Concurrency/bcMutex.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include <mutex>

namespace black_cat
{
	namespace core_platform
	{
		template<>
		struct bc_platform_spin_mutex_pack< bc_platform::win32 >
		{
			bc_atomic_flag m_locked;
		};
		
		template<>
		struct bc_platform_mutex_pack< bc_platform::win32 >
		{
			CRITICAL_SECTION m_critical_section;
#ifdef BC_DEBUG
			bc_atomic_flag m_flag;
#endif
		};

		template<>
		struct bc_platform_timed_mutex_pack< bc_platform::win32 >
		{
			std::timed_mutex m_mutex;
		};

		template<>
		struct bc_platform_recursive_mutex_pack< bc_platform::win32 >
		{
			CRITICAL_SECTION m_critical_section;
		};

		template<>
		struct bc_platform_recursive_timed_mutex_pack< bc_platform::win32 >
		{
			std::recursive_timed_mutex m_mutex;
		};

		template<>
		struct bc_platform_shared_mutex_pack< bc_platform::win32 >
		{
			SRWLOCK m_lock;
		};

		template<>
		struct bc_platform_hybrid_mutex_pack< bc_platform::win32 >
		{
			bc_atomic< bcINT32 > m_flag;
#ifdef BC_DEBUG
			bc_atomic< bcUINT32 > m_thread_id;
#endif

			bc_platform_hybrid_mutex_pack()
				: m_flag(0)
#ifdef BC_DEBUG
				, m_thread_id(0)
#endif
			{
			}
		};
	}
}