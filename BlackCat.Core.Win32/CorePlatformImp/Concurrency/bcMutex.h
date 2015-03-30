// [10/11/2014 MRB]

#pragma once

#include <mutex>
#include "CorePlatform/bcCorePlatformUtility.h"
#include "CorePlatform/Concurrency/bcMutex.h"
#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcThread.h"

namespace black_cat
{
	namespace core_platform
	{
		template<>
		struct bc_mutex_pack< bc_platform::win32 >
		{
			std::mutex m_mutex;
		};

		template<>
		struct bc_timed_mutex_pack< bc_platform::win32 >
		{
			std::timed_mutex m_mutex;
		};

		template<>
		struct bc_recursive_mutex_pack< bc_platform::win32 >
		{
			std::recursive_mutex m_mutex;
		};

		template<>
		struct bc_recursive_timed_mutex_pack< bc_platform::win32 >
		{
			std::recursive_timed_mutex m_mutex;
		};

		template<>
		struct bc_shared_mutex_pack< bc_platform::win32 >
		{
			SRWLOCK m_lock;
		};

		template<>
		struct bc_hybrid_mutex_pack< bc_platform::win32 >
		{
			bc_atomic< bcINT32 > m_flag;
#ifdef BC_DEBUG
			bc_atomic< bcUINT32 > m_thread_id;
#endif

			bc_hybrid_mutex_pack()
				: m_flag(0)
#ifdef BC_DEBUG
				, m_thread_id(0)
#endif
			{
			}
		};

		template<>
		inline bc_mutex_proxy< bc_platform::win32 >::bc_mutex_proxy() = default;

		template<>
		inline bc_mutex_proxy< bc_platform::win32 >::~bc_mutex_proxy() = default;

		template<>
		inline void bc_mutex_proxy< bc_platform::win32 >::lock()
		{
			m_pack.m_mutex.lock(); 
		};

		template<>
		inline void bc_mutex_proxy< bc_platform::win32 >::unlock() noexcept(true)
		{ 
			m_pack.m_mutex.unlock();
		};

		template<>
		inline bool bc_mutex_proxy< bc_platform::win32 >::try_lock() noexcept(true)
		{ 
			return m_pack.m_mutex.try_lock(); 
		};

		template<>
		inline bc_timed_mutex_proxy< bc_platform::win32 >::bc_timed_mutex_proxy() = default;

		template<>
		inline bc_timed_mutex_proxy< bc_platform::win32 >::~bc_timed_mutex_proxy() = default;

		template<>
		inline void bc_timed_mutex_proxy< bc_platform::win32 >::lock()
		{
			m_pack.m_mutex.lock();
		};

		template<>
		inline void bc_timed_mutex_proxy< bc_platform::win32 >::unlock() noexcept(true)
		{
			m_pack.m_mutex.unlock();
		};

		template<>
		inline bool bc_timed_mutex_proxy< bc_platform::win32 >::try_lock() noexcept(true)
		{
			return m_pack.m_mutex.try_lock();
		};

		template<>
		inline bool bc_timed_mutex_proxy< bc_platform::win32 >::try_lock_for(const bcUINT64 p_nano)
		{
			return m_pack.m_mutex.try_lock_for(std::chrono::nanoseconds(p_nano));
		};

		template<>
		inline bc_recursive_mutex_proxy< bc_platform::win32 >::bc_recursive_mutex_proxy() = default;

		template<>
		inline bc_recursive_mutex_proxy< bc_platform::win32 >::~bc_recursive_mutex_proxy() = default;

		template<>
		inline void bc_recursive_mutex_proxy< bc_platform::win32 >::lock()
		{
			m_pack.m_mutex.lock();
		};

		template<>
		inline void bc_recursive_mutex_proxy< bc_platform::win32 >::unlock() noexcept(true)
		{
			m_pack.m_mutex.unlock();
		};

		template<>
		inline bool bc_recursive_mutex_proxy< bc_platform::win32 >::try_lock() noexcept(true)
		{
			return m_pack.m_mutex.try_lock();
		};

		template<>
		inline bc_recursive_timed_mutex_proxy< bc_platform::win32 >::bc_recursive_timed_mutex_proxy() = default;

		template<>
		inline bc_recursive_timed_mutex_proxy< bc_platform::win32 >::~bc_recursive_timed_mutex_proxy() = default;

		template<>
		inline void bc_recursive_timed_mutex_proxy< bc_platform::win32 >::lock()
		{
			m_pack.m_mutex.lock();
		};

		template<>
		inline void bc_recursive_timed_mutex_proxy< bc_platform::win32 >::unlock() noexcept(true)
		{
			m_pack.m_mutex.unlock();
		};

		template<>
		inline bool bc_recursive_timed_mutex_proxy< bc_platform::win32 >::try_lock() noexcept(true)
		{
			return m_pack.m_mutex.try_lock();
		};

		template<>
		inline bool bc_recursive_timed_mutex_proxy< bc_platform::win32 >::try_lock_for(const bcUINT64 p_nano)
		{
			return m_pack.m_mutex.try_lock_for(std::chrono::nanoseconds(p_nano));
		};
		
		template<>
		inline bc_shared_mutex_proxy< bc_platform::win32 >::bc_shared_mutex_proxy()
		{
			InitializeSRWLock(&m_pack.m_lock);
		};

		template<>
		inline bc_shared_mutex_proxy< bc_platform::win32 >::~bc_shared_mutex_proxy()
		{
			// TODO check for any need to release lock
		};

		template<>
		inline void bc_shared_mutex_proxy< bc_platform::win32 >::lock()
		{
			AcquireSRWLockExclusive(&m_pack.m_lock);
		};

		template<>
		inline void bc_shared_mutex_proxy< bc_platform::win32 >::lock_shared()
		{
			AcquireSRWLockShared(&m_pack.m_lock);
		};

		template<>
		inline void bc_shared_mutex_proxy< bc_platform::win32 >::unlock()
		{
			ReleaseSRWLockExclusive(&m_pack.m_lock);
		};

		template<>
		inline void bc_shared_mutex_proxy< bc_platform::win32 >::unlock_shared()
		{
			ReleaseSRWLockShared(&m_pack.m_lock);
		};

		template<>
		inline bool bc_shared_mutex_proxy< bc_platform::win32 >::try_lock()
		{
			return TryAcquireSRWLockExclusive(&m_pack.m_lock);
		};

		template<>
		inline bool bc_shared_mutex_proxy< bc_platform::win32 >::try_lock_shared()
		{
			return TryAcquireSRWLockShared(&m_pack.m_lock);
		};

		template<>
		inline bool bc_shared_mutex_proxy< bc_platform::win32 >::try_lock_for(const bcUINT64 p_nano)
		{
			bcAssert(false);

			return false;
		};

		template<>
		inline bool bc_shared_mutex_proxy< bc_platform::win32 >::try_lock_shared_for(const bcUINT64 p_nano)
		{
			bcAssert(false);

			return false;
		};

		template<>
		inline bc_hybrid_mutex_proxy< bc_platform::win32 >::bc_hybrid_mutex_proxy()
			: m_pack()
		{
		};

		template<>
		inline bc_hybrid_mutex_proxy< bc_platform::win32 >::~bc_hybrid_mutex_proxy() noexcept(true) = default;

		template<>
		inline void bc_hybrid_mutex_proxy< bc_platform::win32 >::lock()
		{
			lock(bc_lock_operation::medium);
		};

		template<>
		inline void bc_hybrid_mutex_proxy< bc_platform::win32 >::lock(bc_lock_operation p_lock_operation)
		{
			bcINT32 l_new_iteration_count = static_cast< bcINT32 >(p_lock_operation);
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
				if (m_pack.m_flag.compare_exchange_strong(&l_expected,
					    l_new_iteration_count,
					    bc_memory_order::acquire,
					    bc_memory_order::relaxed))
				{
#ifdef BC_DEBUG
					m_pack.m_thread_id.store(bc_thread::current_thread_id(), bc_memory_order::relaxed);
#endif
					return;
				}

				// update state variables if new thread has acquired lock
				if (l_iteration_count != l_expected)
				{
					l_iteration = std::abs(l_iteration + (l_expected - l_iteration_count));
					l_iteration_count = l_expected;
					l_half_iteration_count = l_iteration_count / 3;
				}
				l_expected = 0;

				--l_iteration;

				if (l_iteration > l_half_iteration_count)
				{
					bc_thread::current_thread_yield();
					continue;
				}

				if (l_iteration != 0)
				{
					bc_thread::current_thread_sleep_for(0);
					continue;
				}

				bc_thread::current_thread_yield_switch();
				l_iteration = l_iteration_count;
			}
		};

		template<>
		inline void bc_hybrid_mutex_proxy< bc_platform::win32 >::unlock() noexcept(true)
		{
#ifdef BC_DEBUG
			// only those thread that acquired the lock can unlock it
			bcAssert(m_pack.m_thread_id.load(bc_memory_order::relaxed) == bc_thread::current_thread_id());
			m_pack.m_thread_id.store(0U, bc_memory_order::relaxed);
#endif
			m_pack.m_flag.store(0, bc_memory_order::release);
		};

		template<>
		inline bool bc_hybrid_mutex_proxy< bc_platform::win32 >::try_lock() noexcept(true)
		{
			return try_lock(bc_lock_operation::medium);
		};

		template<>
		inline bool bc_hybrid_mutex_proxy< bc_platform::win32 >::try_lock(bc_lock_operation p_lock_operation) noexcept(true)
		{
			bcINT32 l_new_iteration_count = static_cast< bcINT32 >(p_lock_operation);
			bcINT32 l_expected = 0;
#ifdef BC_DEBUG
			// prevent double lock on same thread
			bcAssert(m_pack.m_thread_id.load(bc_memory_order::relaxed) != bc_thread::current_thread_id());
#endif
			bool l_result = m_pack.m_flag.compare_exchange_strong(&l_expected,
				                l_new_iteration_count,
				                bc_memory_order::acquire,
				                bc_memory_order::relaxed);
#ifdef BC_DEBUG
			if (l_result)
			{
				m_pack.m_thread_id.store(bc_thread::current_thread_id(), bc_memory_order::relaxed);
			}
#endif
			return l_result;
		};
	}
}