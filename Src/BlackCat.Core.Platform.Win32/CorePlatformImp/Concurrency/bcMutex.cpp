// [10/28/2016 MRB]

#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "CorePlatformImp/bcExport.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Concurrency/bcThread.h"

namespace black_cat
{
	namespace core_platform
	{
		template<>
		BC_COREPLATFORMIMP_DLL
		bc_platform_mutex< bc_platform::win32 >::bc_platform_mutex()
		{
			InitializeCriticalSection(&m_pack.m_critical_section);
			m_pack.m_flag.clear(bc_memory_order::relaxed);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bc_platform_mutex< bc_platform::win32 >::~bc_platform_mutex()
		{
			DeleteCriticalSection(&m_pack.m_critical_section);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		void bc_platform_mutex< bc_platform::win32 >::lock()
		{
			EnterCriticalSection(&m_pack.m_critical_section);

			if (m_pack.m_flag.test_and_set(bc_memory_order::relaxed))
			{
				bcAssert(false, "Recursive call on non-recursive mutex");
			}
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		void bc_platform_mutex< bc_platform::win32 >::unlock() noexcept
		{
			m_pack.m_flag.clear(bc_memory_order::relaxed);

			LeaveCriticalSection(&m_pack.m_critical_section);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_mutex< bc_platform::win32 >::try_lock() noexcept
		{
			const bool l_result = TryEnterCriticalSection(&m_pack.m_critical_section);

			if (l_result)
			{
				if (m_pack.m_flag.test_and_set(bc_memory_order::relaxed))
				{
					bcAssert(false, "Recursive call on non-recursive mutex");
				}
			}

			return l_result;
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bc_platform_timed_mutex< bc_platform::win32 >::bc_platform_timed_mutex() = default;

		template<>
		BC_COREPLATFORMIMP_DLL
		bc_platform_timed_mutex< bc_platform::win32 >::~bc_platform_timed_mutex() = default;

		template<>
		BC_COREPLATFORMIMP_DLL
		void bc_platform_timed_mutex< bc_platform::win32 >::lock()
		{
			m_pack.m_mutex.lock();
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		void bc_platform_timed_mutex< bc_platform::win32 >::unlock() noexcept
		{
			m_pack.m_mutex.unlock();
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_timed_mutex< bc_platform::win32 >::try_lock() noexcept
		{
			return m_pack.m_mutex.try_lock();
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_timed_mutex< bc_platform::win32 >::try_lock_for(bcUINT64 p_nano)
		{
			return m_pack.m_mutex.try_lock_for(std::chrono::nanoseconds(p_nano));
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bc_platform_recursive_mutex< bc_platform::win32 >::bc_platform_recursive_mutex()
		{
			InitializeCriticalSection(&m_pack.m_critical_section);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bc_platform_recursive_mutex< bc_platform::win32 >::~bc_platform_recursive_mutex()
		{
			DeleteCriticalSection(&m_pack.m_critical_section);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		void bc_platform_recursive_mutex< bc_platform::win32 >::lock()
		{
			EnterCriticalSection(&m_pack.m_critical_section);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		void bc_platform_recursive_mutex< bc_platform::win32 >::unlock() noexcept
		{
			LeaveCriticalSection(&m_pack.m_critical_section);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_recursive_mutex< bc_platform::win32 >::try_lock() noexcept
		{
			return TryEnterCriticalSection(&m_pack.m_critical_section);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bc_platform_recursive_timed_mutex< bc_platform::win32 >::bc_platform_recursive_timed_mutex() = default;

		template<>
		BC_COREPLATFORMIMP_DLL
		bc_platform_recursive_timed_mutex< bc_platform::win32 >::~bc_platform_recursive_timed_mutex() = default;

		template<>
		BC_COREPLATFORMIMP_DLL
		void bc_platform_recursive_timed_mutex< bc_platform::win32 >::lock()
		{
			m_pack.m_mutex.lock();
		};

		template<>
		BC_COREPLATFORMIMP_DLL
		void bc_platform_recursive_timed_mutex< bc_platform::win32 >::unlock() noexcept
		{
			m_pack.m_mutex.unlock();
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_recursive_timed_mutex< bc_platform::win32 >::try_lock() noexcept
		{
			return m_pack.m_mutex.try_lock();
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_recursive_timed_mutex< bc_platform::win32 >::try_lock_for(bcUINT64 p_nano)
		{
			return m_pack.m_mutex.try_lock_for(std::chrono::nanoseconds(p_nano));
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bc_platform_shared_mutex< bc_platform::win32 >::bc_platform_shared_mutex()
		{
			InitializeSRWLock(&m_pack.m_lock);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bc_platform_shared_mutex< bc_platform::win32 >::~bc_platform_shared_mutex()
		{
			// TODO check for any need to release lock
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		void bc_platform_shared_mutex< bc_platform::win32 >::lock()
		{
			AcquireSRWLockExclusive(&m_pack.m_lock);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		void bc_platform_shared_mutex< bc_platform::win32 >::lock_shared()
		{
			AcquireSRWLockShared(&m_pack.m_lock);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		void bc_platform_shared_mutex< bc_platform::win32 >::unlock()
		{
			ReleaseSRWLockExclusive(&m_pack.m_lock);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		void bc_platform_shared_mutex< bc_platform::win32 >::unlock_shared()
		{
			ReleaseSRWLockShared(&m_pack.m_lock);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_shared_mutex< bc_platform::win32 >::try_lock()
		{
			return TryAcquireSRWLockExclusive(&m_pack.m_lock);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_shared_mutex< bc_platform::win32 >::try_lock_shared()
		{
			return TryAcquireSRWLockShared(&m_pack.m_lock);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_shared_mutex< bc_platform::win32 >::try_lock_for(bcUINT64 p_nano)
		{
			bcAssert(false);

			return false;
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_shared_mutex< bc_platform::win32 >::try_lock_shared_for(bcUINT64 p_nano)
		{
			bcAssert(false);

			return false;
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bc_platform_hybrid_mutex< bc_platform::win32 >::bc_platform_hybrid_mutex()
			: m_pack()
		{
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bc_platform_hybrid_mutex< bc_platform::win32 >::~bc_platform_hybrid_mutex()
		{
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		void bc_platform_hybrid_mutex< bc_platform::win32 >::lock()
		{
			lock(bc_lock_operation::medium);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		void bc_platform_hybrid_mutex< bc_platform::win32 >::lock(bc_lock_operation p_lock_operation)
		{
			const bcINT32 l_new_iteration_count = static_cast< bcINT32 >(p_lock_operation);
			bcINT32 l_expected = 0;
			bcINT32 l_iteration_count = 0;
			bcINT32 l_half_iteration_count = 0;
			bcINT32 l_iteration = 0;

#ifdef BC_DEBUG
			// prevent double lock on same thread
			bcAssert(m_pack.m_thread_id.load(bc_memory_order::relaxed) != bc_thread::current_thread_id());
#endif

			while (true)
			{
				if (m_pack.m_flag.compare_exchange_strong
				(
					&l_expected,
					l_new_iteration_count,
					bc_memory_order::acquire,
					bc_memory_order::relaxed
				))
				{
#ifdef BC_DEBUG
					m_pack.m_thread_id.store(bc_thread::current_thread_id(), bc_memory_order::relaxed);
#endif
					return;
				}

				// update state variables if another thread has acquired lock
				if (l_iteration_count != l_expected)
				{
					l_iteration = std::abs(l_iteration + (l_expected - l_iteration_count));
					l_iteration_count = l_expected;
					l_half_iteration_count = l_iteration_count / 3;
				}
				l_expected = 0;

				--l_iteration;

				if (l_iteration >= l_half_iteration_count)
				{
					bc_thread::current_thread_yield();
					continue;
				}

				if (l_iteration < l_half_iteration_count && l_iteration > 0)
				{
					bc_thread::current_thread_sleep_for(std::chrono::nanoseconds(0));
					continue;
				}

				bc_thread::current_thread_yield_switch();
				l_iteration = l_iteration_count;
			}
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		void bc_platform_hybrid_mutex< bc_platform::win32 >::unlock() noexcept
		{
#ifdef BC_DEBUG
			// only those thread that acquired the lock can unlock it
			bcAssert(m_pack.m_thread_id.load(bc_memory_order::relaxed) == bc_thread::current_thread_id());
			m_pack.m_thread_id.store(0U, bc_memory_order::relaxed);
#endif
			m_pack.m_flag.store(0, bc_memory_order::release);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_hybrid_mutex< bc_platform::win32 >::try_lock() noexcept
		{
			return try_lock(bc_lock_operation::medium);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_hybrid_mutex< bc_platform::win32 >::try_lock(bc_lock_operation p_lock_operation) noexcept
		{
			bcINT32 l_new_iteration_count = static_cast< bcINT32 >(p_lock_operation);
			bcINT32 l_expected = 0;
#ifdef BC_DEBUG
			// prevent double lock on same thread
			bcAssert(m_pack.m_thread_id.load(bc_memory_order::relaxed) != bc_thread::current_thread_id());
#endif
			bool l_result = m_pack.m_flag.compare_exchange_strong
			(
				&l_expected,
				l_new_iteration_count,
				bc_memory_order::acquire,
				bc_memory_order::relaxed
			);
#ifdef BC_DEBUG
			if (l_result)
			{
				m_pack.m_thread_id.store(bc_thread::current_thread_id(), bc_memory_order::relaxed);
			}
#endif
			return l_result;
		}
	}
}