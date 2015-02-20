// [10/13/2014 MRB]

#pragma once

#include <atomic>
#include "CorePlatform/bcCorePlatformUtility.h"
#include "CorePlatform/Concurrency/bcConcurrencyDef.h"
#include "CorePlatform/Concurrency/bcAtomicProvider.h"
#include "CorePlatformImp/CoreWin32PCH.h"

namespace black_cat
{
	namespace core_platform
	{
		template<>
		class bc_atomic_flag_provider< bc_platform::win32 > : public bc_atomic_flag_provider< bc_platform::unknown >
		{
			typedef bc_atomic_flag_provider< bc_platform::win32 > this_type;

		public:
			bc_atomic_flag_provider() noexcept(true)
			{
			}

			bc_atomic_flag_provider(bool p_flag) noexcept(true)
			{
				if (p_flag)
					m_flag.test_and_set();
				else
					m_flag.clear();
			}

			~bc_atomic_flag_provider() noexcept(true)
			{
			}

			bcInline bool test_and_set() volatile noexcept(true)
			{
				return m_flag.test_and_set();
			}

			bcInline bool test_and_set() noexcept(true)
			{
				return m_flag.test_and_set();
			}

			bcInline bool test_and_set(bc_memory_order p_order) volatile noexcept(true)
			{
				return m_flag.test_and_set(static_cast<std::memory_order>(p_order));
			}

			bcInline bool test_and_set(bc_memory_order p_order) noexcept(true)
			{
				return m_flag.test_and_set(static_cast<std::memory_order>(p_order));
			}

			bcInline void clear() volatile noexcept(true)
			{
				m_flag.clear();
			}

			bcInline void clear() noexcept(true)
			{
				m_flag.clear();
			}

			bcInline void clear(bc_memory_order p_order) volatile noexcept(true)
			{
				m_flag.clear(static_cast<std::memory_order>(p_order));
			}

			bcInline void clear(bc_memory_order p_order) noexcept(true)
			{
				m_flag.clear(static_cast<std::memory_order>(p_order));
			}

		protected:

		private:
			std::atomic_flag m_flag;

		};

		template< typename T >
		class bc_atomic_provider< bc_platform::win32, T > : public bc_atomic_provider< bc_platform::unknown, T >
		{
			typedef bc_atomic_provider< bc_platform::win32, T > this_type;

		public:
			bc_atomic_provider() noexcept(true)
			{
			}

			~bc_atomic_provider() noexcept(true)
			{
			}

			bc_atomic_provider(T p_value) noexcept(true)
				: m_atomic(p_value)
			{
			}

			bcInline bool is_lock_free() const volatile noexcept(true)
			{
				return m_atomic.is_lock_free();
			}

			bcInline bool is_lock_free() const noexcept(true)
			{
				return m_atomic.is_lock_free();
			}

			bcInline T exchange(T p_value) volatile noexcept(true)
			{
				return m_atomic.exchange(p_value);
			}

			bcInline T exchange(T p_value) noexcept(true)
			{
				return m_atomic.exchange(p_value);
			}

			bcInline T exchange(T p_value, bc_memory_order p_order) volatile noexcept(true)
			{
				return m_atomic.exchange(p_value, static_cast<std::memory_order>(p_order));
			}

			bcInline T exchange(T p_value, bc_memory_order p_order) noexcept(true)
			{
				return m_atomic.exchange(p_value, static_cast<std::memory_order>(p_order));
			}

			bcInline void store(T p_value) volatile noexcept(true)
			{
				m_atomic.store(p_value);
			}

			bcInline void store(T p_value) noexcept(true)
			{
				m_atomic.store(p_value);
			}

			bcInline void store(T p_value, bc_memory_order p_order) volatile noexcept(true)
			{
				m_atomic.store(p_value, static_cast<std::memory_order>(p_order));
			}

			bcInline void store(T p_value, bc_memory_order p_order) noexcept(true)
			{
				m_atomic.store(p_value, static_cast<std::memory_order>(p_order));
			}

			bcInline T load() const volatile noexcept(true)
			{
				return m_atomic.load();
			}

			bcInline T load() const noexcept(true)
			{
				return m_atomic.load();
			}

			bcInline T load(bc_memory_order p_order) const volatile noexcept(true)
			{
				return m_atomic.load(static_cast<std::memory_order>(p_order));
			}

			bcInline T load(bc_memory_order p_order) const noexcept(true)
			{
				return m_atomic.load(static_cast<std::memory_order>(p_order));
			}

			bcInline T fetch_add(T p_to_add) volatile noexcept(true)
			{
				static_assert(!std::is_same< T, bool >::value, "atomic fetch_add is not supported with bool");

				return m_atomic.fetch_add(p_to_add);
			}

			bcInline T fetch_add(T p_to_add) noexcept(true)
			{
				static_assert(!std::is_same< T, bool >::value, "atomic fetch_add is not supported with bool");

				return m_atomic.fetch_add(p_to_add);
			}

			bcInline T fetch_add(T p_to_add, bc_memory_order p_order) volatile noexcept(true)
			{
				static_assert(!std::is_same< T, bool >::value, "atomic fetch_add is not supported with bool");

				return m_atomic.fetch_add(p_to_add, static_cast<std::memory_order>(p_order));
			}

			bcInline T fetch_add(T p_to_add, bc_memory_order p_order) noexcept(true)
			{
				static_assert(!std::is_same< T, bool >::value, "atomic fetch_add is not supported with bool");

				return m_atomic.fetch_add(p_to_add, static_cast<std::memory_order>(p_order));
			}

			bcInline T fetch_sub(T p_to_sub) volatile noexcept(true)
			{
				static_assert(!std::is_same< T, bool >::value, "atomic fetch_sub is not supported with bool");

				return m_atomic.fetch_sub(p_to_sub);
			}

			bcInline T fetch_sub(T p_to_sub) noexcept(true)
			{
				static_assert(!std::is_same< T, bool >::value, "atomic fetch_sub is not supported with bool");

				return m_atomic.fetch_sub(p_to_sub);
			}

			bcInline T fetch_sub(T p_to_sub, bc_memory_order p_order) volatile noexcept(true)
			{
				static_assert(!std::is_same< T, bool >::value, "atomic fetch_sub is not supported with bool");

				return m_atomic.fetch_sub(p_to_sub, static_cast<std::memory_order>(p_order));
			}

			bcInline T fetch_sub(T p_to_sub, bc_memory_order p_order) noexcept(true)
			{
				static_assert(!std::is_same< T, bool >::value, "atomic fetch_sub is not supported with bool");

				return m_atomic.fetch_sub(p_to_sub, static_cast<std::memory_order>(p_order));
			}

			bcInline T fetch_and(T p_to_and) volatile noexcept(true)
			{
				static_assert(!std::is_same< T, bool >::value, "atomic fetch_and is not supported with bool");
				static_assert(!std::is_integral< T >::value, "atomic fetch_and is not supported with non integral types");

				return std::atomic_fetch_and(&m_atomic, p_to_and);
			}

			bcInline T fetch_and(T p_to_and) noexcept(true)
			{
				static_assert(!std::is_same< T, bool >::value, "atomic fetch_and is not supported with bool");
				static_assert(!std::is_integral< T >::value, "atomic fetch_and is not supported with non integral types");

				return std::atomic_fetch_and(&m_atomic, p_to_and);
			}

			bcInline T fetch_and(T p_to_and, bc_memory_order p_order) volatile noexcept(true)
			{
				static_assert(!std::is_same< T, bool >::value, "atomic fetch_and is not supported with bool");
				static_assert(!std::is_integral< T >::value, "atomic fetch_and is not supported with non integral types");

				return std::atomic_fetch_and_explicit(&m_atomic, p_to_and, static_cast<std::memory_order>(p_order));
			}

			bcInline T fetch_and(T p_to_and, bc_memory_order p_order) noexcept(true)
			{
				static_assert(!std::is_same< T, bool >::value, "atomic fetch_and is not supported with bool");
				static_assert(!std::is_integral< T >::value, "atomic fetch_and is not supported with non integral types");

				return std::atomic_fetch_and_explicit(&m_atomic, p_to_and, static_cast<std::memory_order>(p_order));
			}

			bcInline T fetch_or(T p_to_or) volatile noexcept(true)
			{
				static_assert(!std::is_same< T, bool >::value, "atomic fetch_or is not supported with bool");
				static_assert(!std::is_integral< T >::value, "atomic fetch_or is not supported with non integral types");

				return std::atomic_fetch_or(&m_atomic, p_to_or);
			}

			bcInline T fetch_or(T p_to_or) noexcept(true)
			{
				static_assert(!std::is_same< T, bool >::value, "atomic fetch_or is not supported with bool");
				static_assert(!std::is_integral< T >::value, "atomic fetch_or is not supported with non integral types");

				return std::atomic_fetch_or(&m_atomic, p_to_or);
			}

			bcInline T fetch_or(T p_to_or, bc_memory_order p_order) volatile noexcept(true)
			{
				static_assert(!std::is_same< T, bool >::value, "atomic fetch_or is not supported with bool");
				static_assert(!std::is_integral< T >::value, "atomic fetch_or is not supported with non integral types");

				return std::atomic_fetch_or_explicit(&m_atomic, p_to_or, static_cast<std::memory_order>(p_order));
			}

			bcInline T fetch_or(T p_to_or, bc_memory_order p_order) noexcept(true)
			{
				static_assert(!std::is_same< T, bool >::value, "atomic fetch_or is not supported with bool");
				static_assert(!std::is_integral< T >::value, "atomic fetch_or is not supported with non integral types");

				return std::atomic_fetch_or_explicit(&m_atomic, p_to_or, static_cast<std::memory_order>(p_order));
			}

			bcInline bool compare_exchange_strong(T* p_expected_value, T p_new_value) volatile noexcept(true)
			{
				return m_atomic.compare_exchange_strong(*p_expected_value, p_new_value);
			}

			bcInline bool compare_exchange_strong(T* p_expected_value, T p_new_value) noexcept(true)
			{
				return m_atomic.compare_exchange_strong(*p_expected_value, p_new_value);
			}

			bcInline bool compare_exchange_strong(T* p_expected_value, T p_new_value, bc_memory_order p_order) volatile noexcept(true)
			{
				return m_atomic.compare_exchange_strong(*p_expected_value, p_new_value, static_cast<std::memory_order>(p_order));
			}

			bcInline bool compare_exchange_strong(T* p_expected_value, T p_new_value, bc_memory_order p_order) noexcept(true)
			{
				return m_atomic.compare_exchange_strong(*p_expected_value, p_new_value, static_cast<std::memory_order>(p_order));
			}

			bcInline bool compare_exchange_strong(T* p_expected_value,
				T p_new_value,
				bc_memory_order p_success_order,
				bc_memory_order p_failure_order) volatile noexcept(true)
			{
				return m_atomic.compare_exchange_strong(*p_expected_value,
					p_new_value,
					static_cast<std::memory_order>(p_success_order),
					static_cast<std::memory_order>(p_failure_order));
			}

			bcInline bool compare_exchange_strong(T* p_expected_value,
				T p_new_value,
				bc_memory_order p_success_order,
				bc_memory_order p_failure_order) noexcept(true)
			{
				return m_atomic.compare_exchange_strong(*p_expected_value,
					p_new_value,
					static_cast<std::memory_order>(p_success_order),
					static_cast<std::memory_order>(p_failure_order));
			}

			bcInline bool compare_exchange_weak(T* p_expected_value, T p_new_value) volatile noexcept(true)
			{
				return m_atomic.compare_exchange_weak(*p_expected_value, p_new_value);
			}

			bcInline bool compare_exchange_weak(T* p_expected_value, T p_new_value) noexcept(true)
			{
				return m_atomic.compare_exchange_weak(*p_expected_value, p_new_value);
			}

			bcInline bool compare_exchange_weak(T* p_expected_value, T p_new_value, bc_memory_order p_order) volatile noexcept(true)
			{
				return m_atomic.compare_exchange_weak(*p_expected_value, p_new_value, static_cast<std::memory_order>(p_order));
			}

			bcInline bool compare_exchange_weak(T* p_expected_value, T p_new_value, bc_memory_order p_order) noexcept(true)
			{
				return m_atomic.compare_exchange_weak(*p_expected_value, p_new_value, static_cast<std::memory_order>(p_order));
			}

			bcInline bool compare_exchange_weak(T* p_expected_value,
				T p_new_value,
				bc_memory_order p_success_order,
				bc_memory_order p_failure_order) volatile noexcept(true)
			{
				return m_atomic.compare_exchange_weak(*p_expected_value,
					p_new_value,
					static_cast<std::memory_order>(p_success_order),
					static_cast<std::memory_order>(p_failure_order));
			}

			bcInline bool compare_exchange_weak(T* p_expected_value,
				T p_new_value,
				bc_memory_order p_success_order,
				bc_memory_order p_failure_order) noexcept(true)
			{
				return m_atomic.compare_exchange_weak(*p_expected_value,
					p_new_value,
					static_cast<std::memory_order>(p_success_order),
					static_cast<std::memory_order>(p_failure_order));
			}

		protected:

		private:
			std::atomic< T > m_atomic;

		};
	}
}