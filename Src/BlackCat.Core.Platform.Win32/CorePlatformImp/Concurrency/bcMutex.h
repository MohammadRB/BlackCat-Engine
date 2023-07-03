// [11/10/2014 MRB]

#pragma once

#include <mutex>
#include "CorePlatform/Concurrency/bcMutex.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcThread.h"
#include "CorePlatformImp/bcUtility.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_spin_mutex_pack<bc_platform::win32>
		{
			CRITICAL_SECTION m_critical_section;
#ifdef BC_DEBUG
			bc_atomic_flag m_flag;
#endif
		};
		
		template<>
		struct bc_platform_mutex_pack<bc_platform::win32>
		{
			CRITICAL_SECTION m_critical_section;
#ifdef BC_DEBUG
			bc_atomic_flag m_flag;
#endif
		};

		template<>
		struct bc_platform_timed_mutex_pack<bc_platform::win32>
		{
			std::timed_mutex m_mutex;
		};

		template<>
		struct bc_platform_recursive_mutex_pack<bc_platform::win32>
		{
			CRITICAL_SECTION m_critical_section;
		};

		template<>
		struct bc_platform_recursive_timed_mutex_pack<bc_platform::win32>
		{
			std::recursive_timed_mutex m_mutex;
		};

		template<>
		struct bc_platform_shared_mutex_pack<bc_platform::win32>
		{
			SRWLOCK m_lock;
		};

		template<>
		struct bc_platform_hybrid_mutex_pack<bc_platform::win32>
		{
			bc_atomic<bcINT32> m_flag;
#ifdef BC_DEBUG
			bc_atomic<bcUINT32> m_thread_id;
#endif

			bc_platform_hybrid_mutex_pack()
				: m_flag(0)
#ifdef BC_DEBUG
				, m_thread_id(0)
#endif
			{
			}
		};

		template<>
		inline bc_platform_spin_mutex<bc_platform::win32>::bc_platform_spin_mutex()
		{
			win_call(static_cast<bool>(InitializeCriticalSectionAndSpinCount(&m_pack.m_critical_section, 2000U)));
#ifdef BC_DEBUG
			m_pack.m_flag.clear(bc_memory_order::relaxed);
#endif
		}

		template<>
		inline bc_platform_spin_mutex<bc_platform::win32>::bc_platform_spin_mutex(bcUINT32 p_spin_count)
		{
			win_call(static_cast<bool>(InitializeCriticalSectionAndSpinCount(&m_pack.m_critical_section, p_spin_count)));
#ifdef BC_DEBUG
			m_pack.m_flag.clear(bc_memory_order::relaxed);
#endif
		}

		template<>
		inline bc_platform_spin_mutex<bc_platform::win32>::~bc_platform_spin_mutex()
		{
			DeleteCriticalSection(&m_pack.m_critical_section);
		}

		template<>
		inline void bc_platform_spin_mutex<bc_platform::win32>::change_spin_count(bcUINT32 p_spin_count)
		{
			SetCriticalSectionSpinCount(&m_pack.m_critical_section, p_spin_count);
		}

		template<>
		inline void bc_platform_spin_mutex<bc_platform::win32>::lock()
		{
			EnterCriticalSection(&m_pack.m_critical_section);

#ifdef BC_DEBUG
			if (m_pack.m_flag.test_and_set(bc_memory_order::relaxed))
			{
				BC_ASSERT(false);
			}
#endif
		}

		template<>
		inline void bc_platform_spin_mutex<bc_platform::win32>::unlock() noexcept
		{
#ifdef BC_DEBUG
			m_pack.m_flag.clear(bc_memory_order::relaxed);
#endif

			LeaveCriticalSection(&m_pack.m_critical_section);
		}

		template<>
		inline bool bc_platform_spin_mutex<bc_platform::win32>::try_lock() noexcept
		{
			const bool l_result = TryEnterCriticalSection(&m_pack.m_critical_section);

#ifdef BC_DEBUG
			if (l_result)
			{
				if (m_pack.m_flag.test_and_set(bc_memory_order::relaxed))
				{
					BC_ASSERT(false);
				}
			}
#endif

			return l_result;
		}

		template<>
		inline bc_platform_mutex<bc_platform::win32>::bc_platform_mutex()
		{
			InitializeCriticalSection(&m_pack.m_critical_section);
#ifdef BC_DEBUG
			m_pack.m_flag.clear(bc_memory_order::relaxed);
#endif
		}

		template<>
		inline bc_platform_mutex<bc_platform::win32>::~bc_platform_mutex()
		{
			DeleteCriticalSection(&m_pack.m_critical_section);
		}

		template<>
		inline void bc_platform_mutex<bc_platform::win32>::lock()
		{
			EnterCriticalSection(&m_pack.m_critical_section);

#ifdef BC_DEBUG
			if (m_pack.m_flag.test_and_set(bc_memory_order::relaxed))
			{
				BC_ASSERT(false);
			}
#endif
		}

		template<>
		inline void bc_platform_mutex<bc_platform::win32>::unlock() noexcept
		{
#ifdef BC_DEBUG
			m_pack.m_flag.clear(bc_memory_order::relaxed);
#endif

			LeaveCriticalSection(&m_pack.m_critical_section);
		}

		template<>
		inline bool bc_platform_mutex<bc_platform::win32>::try_lock() noexcept
		{
			const bool l_result = TryEnterCriticalSection(&m_pack.m_critical_section);

#ifdef BC_DEBUG
			if (l_result)
			{
				if (m_pack.m_flag.test_and_set(bc_memory_order::relaxed))
				{
					BC_ASSERT(false);
				}
			}
#endif

			return l_result;
		}

		template<>
		inline bc_platform_timed_mutex<bc_platform::win32>::bc_platform_timed_mutex() = default;

		template<>
		inline bc_platform_timed_mutex<bc_platform::win32>::~bc_platform_timed_mutex() = default;

		template<>
		inline void bc_platform_timed_mutex<bc_platform::win32>::lock()
		{
			m_pack.m_mutex.lock();
		}

		template<>
		inline void bc_platform_timed_mutex<bc_platform::win32>::unlock() noexcept
		{
			m_pack.m_mutex.unlock();
		}

		template<>
		inline bool bc_platform_timed_mutex<bc_platform::win32>::try_lock() noexcept
		{
			return m_pack.m_mutex.try_lock();
		}

		template<>
		inline bool bc_platform_timed_mutex<bc_platform::win32>::try_lock_for(bcUINT64 p_nano)
		{
			return m_pack.m_mutex.try_lock_for(std::chrono::nanoseconds(p_nano));
		}

		template<>
		inline bc_platform_recursive_mutex<bc_platform::win32>::bc_platform_recursive_mutex()
		{
			InitializeCriticalSection(&m_pack.m_critical_section);
		}

		template<>
		inline bc_platform_recursive_mutex<bc_platform::win32>::~bc_platform_recursive_mutex()
		{
			DeleteCriticalSection(&m_pack.m_critical_section);
		}

		template<>
		inline void bc_platform_recursive_mutex<bc_platform::win32>::lock()
		{
			EnterCriticalSection(&m_pack.m_critical_section);
		}

		template<>
		inline void bc_platform_recursive_mutex<bc_platform::win32>::unlock() noexcept
		{
			LeaveCriticalSection(&m_pack.m_critical_section);
		}

		template<>
		inline bool bc_platform_recursive_mutex<bc_platform::win32>::try_lock() noexcept
		{
			return TryEnterCriticalSection(&m_pack.m_critical_section);
		}

		template<>
		inline bc_platform_recursive_timed_mutex<bc_platform::win32>::bc_platform_recursive_timed_mutex() = default;

		template<>
		inline bc_platform_recursive_timed_mutex<bc_platform::win32>::~bc_platform_recursive_timed_mutex() = default;

		template<>
		inline void bc_platform_recursive_timed_mutex<bc_platform::win32>::lock()
		{
			m_pack.m_mutex.lock();
		};

		template<>
		inline void bc_platform_recursive_timed_mutex<bc_platform::win32>::unlock() noexcept
		{
			m_pack.m_mutex.unlock();
		}

		template<>
		inline bool bc_platform_recursive_timed_mutex<bc_platform::win32>::try_lock() noexcept
		{
			return m_pack.m_mutex.try_lock();
		}

		template<>
		inline bool bc_platform_recursive_timed_mutex<bc_platform::win32>::try_lock_for(bcUINT64 p_nano)
		{
			return m_pack.m_mutex.try_lock_for(std::chrono::nanoseconds(p_nano));
		}

		template<>
		inline bc_platform_shared_mutex<bc_platform::win32>::bc_platform_shared_mutex()
		{
			InitializeSRWLock(&m_pack.m_lock);
		}

		template<> // TODO check for any need to release lock
		inline bc_platform_shared_mutex<bc_platform::win32>::~bc_platform_shared_mutex() = default;

		template<>
		inline void bc_platform_shared_mutex<bc_platform::win32>::lock()
		{
			AcquireSRWLockExclusive(&m_pack.m_lock);
		}

		template<>
		inline void bc_platform_shared_mutex<bc_platform::win32>::lock_shared()
		{
			AcquireSRWLockShared(&m_pack.m_lock);
		}

		template<>
		inline void bc_platform_shared_mutex<bc_platform::win32>::unlock()
		{
			ReleaseSRWLockExclusive(&m_pack.m_lock);
		}

		template<>
		inline void bc_platform_shared_mutex<bc_platform::win32>::unlock_shared()
		{
			ReleaseSRWLockShared(&m_pack.m_lock);
		}

		template<>
		inline bool bc_platform_shared_mutex<bc_platform::win32>::try_lock()
		{
			return TryAcquireSRWLockExclusive(&m_pack.m_lock);
		}

		template<>
		inline bool bc_platform_shared_mutex<bc_platform::win32>::try_lock_shared()
		{
			return TryAcquireSRWLockShared(&m_pack.m_lock);
		}

		template<>
		inline bool bc_platform_shared_mutex<bc_platform::win32>::try_lock_for(bcUINT64 p_nano)
		{
			BC_ASSERT(false);

			return false;
		}

		template<>
		inline bool bc_platform_shared_mutex<bc_platform::win32>::try_lock_shared_for(bcUINT64 p_nano)
		{
			BC_ASSERT(false);

			return false;
		}

		template<>
		inline bc_platform_hybrid_mutex<bc_platform::win32>::bc_platform_hybrid_mutex()
			: m_pack()
		{
		}

		template<>
		inline bc_platform_hybrid_mutex<bc_platform::win32>::~bc_platform_hybrid_mutex() = default;
	}
}