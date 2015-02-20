// [10/11/2014 MRB]

#pragma once

#include <mutex>
#include "CorePlatform/bcCorePlatformUtility.h"
#include "CorePlatform/Concurrency/bcMutexProvider.h"
#include "CorePlatformImp/CoreWin32PCH.h"
#include "CorePlatformImp/Concurrency/bcAtomicProvider.h"
#include "CorePlatformImp/Concurrency/bcThreadProvider.h"

namespace black_cat
{
	namespace core_platform
	{
		template<>
		class bc_mutex_provider< bc_platform::win32 > : public bc_mutex_provider< bc_platform::unknown >
		{
		public:
			bc_mutex_provider() = default;

			~bc_mutex_provider() = default;

			bcInline void lock()
			{
				m_mutex.lock(); 
			}

			bcInline void unlock() noexcept(true)
			{ 
				m_mutex.unlock();
			}

			bcInline bool try_lock() noexcept(true)
			{ 
				return m_mutex.try_lock(); 
			}

		protected:

		private:
			std::mutex m_mutex;

		};

		template<>
		class bc_timed_mutex_provider< bc_platform::win32 > : public bc_timed_mutex_provider< bc_platform::unknown >
		{
		public:
			bc_timed_mutex_provider() = default;

			~bc_timed_mutex_provider() = default;

			bcInline void lock()
			{ 
				m_mutex.lock(); 
			}

			bcInline void unlock() noexcept(true)
			{ 
				m_mutex.unlock(); 
			}

			bcInline bool try_lock() noexcept(true)
			{ 
				return m_mutex.try_lock(); 
			}

			bcInline bool try_lock_for(const bcUINT64 p_nano)
			{
				return m_mutex.try_lock_for(std::chrono::nanoseconds(p_nano));
			}

		protected:

		private:
			std::timed_mutex m_mutex;

		};

		template<>
		class bc_recursive_mutex_provider< bc_platform::win32 > : public bc_recursive_mutex_provider< bc_platform::unknown >
		{
		public:
			bc_recursive_mutex_provider() = default;

			~bc_recursive_mutex_provider() = default;

			bcInline void lock()
			{ 
				m_mutex.lock(); 
			}

			bcInline void unlock() noexcept(true)
			{ 
				m_mutex.unlock(); 
			}

			bcInline bool try_lock() noexcept(true)
			{ 
				return m_mutex.try_lock(); 
			}

		protected:

		private:
			std::recursive_mutex m_mutex;

		};

		template<>
		class bc_recursive_timed_mutex_provider< bc_platform::win32 > : public bc_recursive_timed_mutex_provider< bc_platform::unknown >
		{
		public:
			bc_recursive_timed_mutex_provider() = default;

			~bc_recursive_timed_mutex_provider() = default;

			bcInline void lock()
			{ 
				m_mutex.lock(); 
			}

			bcInline void unlock() noexcept(true)
			{ 
				m_mutex.unlock(); 
			}

			bcInline bool try_lock() noexcept(true)
			{ 
				return m_mutex.try_lock(); 
			}

			bcInline bool try_lock_for(const bcUINT64 p_nano)
			{
				return m_mutex.try_lock_for(std::chrono::nanoseconds(p_nano));
			}

		protected:

		private:
			std::recursive_timed_mutex m_mutex;

		};

		template<>
		class bc_shared_mutex_provider<bc_platform::win32> : public bc_shared_mutex_provider< bc_platform::unknown >
		{
		public:
			bc_shared_mutex_provider()
			{
				InitializeSRWLock(&m_lock);
			}

			~bc_shared_mutex_provider()
			{
				// TODO check for any need to release lock
			}

			bcInline void lock()
			{
				AcquireSRWLockExclusive(&m_lock);
			}

			bcInline void lock_shared()
			{
				AcquireSRWLockShared(&m_lock);
			}

			bcInline void unlock()
			{
				ReleaseSRWLockExclusive(&m_lock);
			}

			bcInline void unlock_shared()
			{
				ReleaseSRWLockShared(&m_lock);
			}

			bcInline bool try_lock()
			{
				return TryAcquireSRWLockExclusive(&m_lock);
			}

			bcInline bool try_lock_shared()
			{
				return TryAcquireSRWLockShared(&m_lock);
			}

			bcInline bool try_lock_for(const bcUINT64 p_nano)
			{
				bcAssert(false);

				return false;
			}

			bcInline bool try_lock_shared_for(const bcUINT64 p_nano)
			{
				bcAssert(false);

				return false;
			}

		protected:

		private:
			SRWLOCK m_lock;
		};

		template<>
		class bc_hybrid_mutex_provider< bc_platform::win32 > : public bc_hybrid_mutex_provider< bc_platform::unknown >
		{
		public:
			bc_hybrid_mutex_provider()
				: m_flag(false),
#ifdef BC_DEBUG
				m_thread_id(0U)
#endif
			{
			}

			~bc_hybrid_mutex_provider() = default;

			bcInline void lock()
			{
				lock(bc_lock_operation::medium);
			};

			bcInline void lock(bc_lock_operation p_lock_operation)
			{
				bcINT32 l_new_iteration_count = static_cast<bcINT32>(p_lock_operation);
				bcINT32 l_expected = 0;
				bcINT32 l_iteration_count = 0;
				bcINT32 l_half_iteration_count = 0;
				bcINT32 l_iteration = 0;

#ifdef BC_DEBUG
				// prevent double lock on same thread
				bcAssert(m_thread_id.load(bc_memory_order::relaxed) != bc_thread::current_thread_id());
#endif

				while (true)
				{
					if (m_flag.compare_exchange_strong(&l_expected,
						l_new_iteration_count,
						bc_memory_order::acquire,
						bc_memory_order::relaxed))
					{
#ifdef BC_DEBUG
						m_thread_id.store(bc_thread::current_thread_id(), bc_memory_order::relaxed);
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
						YieldProcessor();
						continue;
					}

					if (l_iteration != 0)
					{
						Sleep(0);
						continue;
					}

					SwitchToThread();
					l_iteration = l_iteration_count;
				}
			};

			bcInline void unlock() noexcept(true)
			{
#ifdef BC_DEBUG
				// only those thread that acquired the lock can unlock it
				bcAssert(m_thread_id.load(bc_memory_order::relaxed) == bc_thread::current_thread_id());
				m_thread_id.store(0U, bc_memory_order::relaxed);
#endif
				m_flag.store(0, bc_memory_order::release);
			};

			bcInline bool try_lock() noexcept(true)
			{
				return try_lock(bc_lock_operation::medium);
			}

			bcInline bool try_lock(bc_lock_operation p_lock_operation) noexcept(true)
			{
				bcINT32 l_new_iteration_count = static_cast<bcINT32>(p_lock_operation);
				bcINT32 l_expected = 0;
#ifdef BC_DEBUG
				// prevent double lock on same thread
				bcAssert(m_thread_id.load(bc_memory_order::relaxed) != bc_thread::current_thread_id());
#endif
				bool l_result = m_flag.compare_exchange_strong(&l_expected, 
					l_new_iteration_count,
					bc_memory_order::acquire,
					bc_memory_order::relaxed);
#ifdef BC_DEBUG
				if (l_result)
				{
					m_thread_id.store(bc_thread::current_thread_id(), bc_memory_order::relaxed);
				}
#endif
				return l_result;
			}

		protected:

		private:
			bc_atomic< bcINT32 > m_flag;
#ifdef BC_DEBUG
			bc_atomic< bcUINT32 > m_thread_id;
#endif

		};
	}
}