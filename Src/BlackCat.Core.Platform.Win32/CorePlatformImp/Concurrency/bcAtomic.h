// [10/13/2014 MRB]

#pragma once

#include <atomic>
#include "CorePlatform/Concurrency/bcConcurrencyDef.h"
#include "CorePlatform/Concurrency/bcAtomic.h"

namespace black_cat
{
	namespace core_platform
	{
		template<>
		struct bc_platform_atomic_flag_pack< bc_platform::win32 >
		{
			std::atomic_flag m_flag;
		};

		template< typename T >
		struct bc_platform_atomic_pack< bc_platform::win32, T >
		{
			bc_platform_atomic_pack()
			{
			}

			explicit bc_platform_atomic_pack(T p_value)
				: m_atomic(p_value)
			{
			}

			std::atomic< T > m_atomic;
		};

		template<>
		inline bc_platform_atomic_flag< bc_platform::win32 >::bc_platform_atomic_flag() noexcept
		{
		}

		template<>
		inline bc_platform_atomic_flag< bc_platform::win32 >::bc_platform_atomic_flag(bool p_flag) noexcept
		{
			if (p_flag)
			{
				m_pack.m_flag.test_and_set();
			}
			else
			{
				m_pack.m_flag.clear();
			}
		}

		template<>
		inline bc_platform_atomic_flag< bc_platform::win32 >::~bc_platform_atomic_flag() noexcept
		{
		}

		template<>
		inline bool bc_platform_atomic_flag< bc_platform::win32 >::test_and_set() volatile noexcept
		{
			return m_pack.m_flag.test_and_set();
		}

		template<>
		inline bool bc_platform_atomic_flag< bc_platform::win32 >::test_and_set() noexcept
		{
			return m_pack.m_flag.test_and_set();
		}

		template<>
		inline bool bc_platform_atomic_flag< bc_platform::win32 >::test_and_set(bc_memory_order p_order) volatile noexcept
		{
			return m_pack.m_flag.test_and_set(static_cast< std::memory_order >(p_order));
		}

		template<>
		inline bool bc_platform_atomic_flag< bc_platform::win32 >::test_and_set(bc_memory_order p_order) noexcept
		{
			return m_pack.m_flag.test_and_set(static_cast< std::memory_order >(p_order));
		}

		template<>
		inline void bc_platform_atomic_flag< bc_platform::win32 >::clear() volatile noexcept
		{
			m_pack.m_flag.clear();
		}

		template<>
		inline void bc_platform_atomic_flag< bc_platform::win32 >::clear() noexcept
		{
			m_pack.m_flag.clear();
		}

		template<>
		inline void bc_platform_atomic_flag< bc_platform::win32 >::clear(bc_memory_order p_order) volatile noexcept
		{
			m_pack.m_flag.clear(static_cast< std::memory_order >(p_order));
		}

		template<>
		inline void bc_platform_atomic_flag< bc_platform::win32 >::clear(bc_memory_order p_order) noexcept
		{
			m_pack.m_flag.clear(static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		bc_platform_atomic< TPlatform, T >::bc_platform_atomic() noexcept
		{
		}

		template< bc_platform TPlatform, typename T >
		bc_platform_atomic< TPlatform, T >::~bc_platform_atomic() noexcept
		{
		}

		template< bc_platform TPlatform, typename T >
		bc_platform_atomic< TPlatform, T >::bc_platform_atomic(T p_value) noexcept
			: m_pack(p_value)
		{
		}

		template< bc_platform TPlatform, typename T >
		bool bc_platform_atomic< TPlatform, T >::is_lock_free() const volatile noexcept
		{
			return m_pack.m_atomic.is_lock_free();
		}

		template< bc_platform TPlatform, typename T >
		bool bc_platform_atomic< TPlatform, T >::is_lock_free() const noexcept
		{
			return m_pack.m_atomic.is_lock_free();
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::exchange(T p_value) volatile noexcept
		{
			return m_pack.m_atomic.exchange(p_value);
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::exchange(T p_value) noexcept
		{
			return m_pack.m_atomic.exchange(p_value);
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::exchange(T p_value, bc_memory_order p_order) volatile noexcept
		{
			return m_pack.m_atomic.exchange(p_value, static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::exchange(T p_value, bc_memory_order p_order) noexcept
		{
			return m_pack.m_atomic.exchange(p_value, static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		void bc_platform_atomic< TPlatform, T >::store(T p_value) volatile noexcept
		{
			m_pack.m_atomic.store(p_value);
		}

		template< bc_platform TPlatform, typename T >
		void bc_platform_atomic< TPlatform, T >::store(T p_value) noexcept
		{
			m_pack.m_atomic.store(p_value);
		}

		template< bc_platform TPlatform, typename T >
		void bc_platform_atomic< TPlatform, T >::store(T p_value, bc_memory_order p_order) volatile noexcept
		{
			m_pack.m_atomic.store(p_value, static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		void bc_platform_atomic< TPlatform, T >::store(T p_value, bc_memory_order p_order) noexcept
		{
			m_pack.m_atomic.store(p_value, static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::load() const volatile noexcept
		{
			return m_pack.m_atomic.load();
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::load() const noexcept
		{
			return m_pack.m_atomic.load();
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::load(bc_memory_order p_order) const volatile noexcept
		{
			return m_pack.m_atomic.load(static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::load(bc_memory_order p_order) const noexcept
		{
			return m_pack.m_atomic.load(static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::fetch_add(T p_to_add) volatile noexcept
		{
			static_assert(!std::is_same< T, bool >::value, "atomic fetch_add is not supported with bool");

			return m_pack.m_atomic.fetch_add(p_to_add);
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::fetch_add(T p_to_add) noexcept
		{
			static_assert(!std::is_same< T, bool >::value, "atomic fetch_add is not supported with bool");

			return m_pack.m_atomic.fetch_add(p_to_add);
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::fetch_add(T p_to_add, bc_memory_order p_order) volatile noexcept
		{
			static_assert(!std::is_same< T, bool >::value, "atomic fetch_add is not supported with bool");

			return m_pack.m_atomic.fetch_add(p_to_add, static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::fetch_add(T p_to_add, bc_memory_order p_order) noexcept
		{
			static_assert(!std::is_same< T, bool >::value, "atomic fetch_add is not supported with bool");

			return m_pack.m_atomic.fetch_add(p_to_add, static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::fetch_sub(T p_to_sub) volatile noexcept
		{
			static_assert(!std::is_same< T, bool >::value, "atomic fetch_sub is not supported with bool");

			return m_pack.m_atomic.fetch_sub(p_to_sub);
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::fetch_sub(T p_to_sub) noexcept
		{
			static_assert(!std::is_same< T, bool >::value, "atomic fetch_sub is not supported with bool");

			return m_pack.m_atomic.fetch_sub(p_to_sub);
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::fetch_sub(T p_to_sub, bc_memory_order p_order) volatile noexcept
		{
			static_assert(!std::is_same< T, bool >::value, "atomic fetch_sub is not supported with bool");

			return m_pack.m_atomic.fetch_sub(p_to_sub, static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::fetch_sub(T p_to_sub, bc_memory_order p_order) noexcept
		{
			static_assert(!std::is_same< T, bool >::value, "atomic fetch_sub is not supported with bool");

			return m_pack.m_atomic.fetch_sub(p_to_sub, static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::fetch_and(T p_to_and) volatile noexcept
		{
			static_assert(!std::is_same< T, bool >::value, "atomic fetch_and is not supported with bool");
			static_assert(std::is_integral< T >::value, "atomic fetch_and is not supported with non integral types");

			return std::atomic_fetch_and(&m_pack.m_atomic, p_to_and);
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::fetch_and(T p_to_and) noexcept
		{
			static_assert(!std::is_same< T, bool >::value, "atomic fetch_and is not supported with bool");
			static_assert(std::is_integral< T >::value, "atomic fetch_and is not supported with non integral types");

			return std::atomic_fetch_and(&m_pack.m_atomic, p_to_and);
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::fetch_and(T p_to_and, bc_memory_order p_order) volatile noexcept
		{
			static_assert(!std::is_same< T, bool >::value, "atomic fetch_and is not supported with bool");
			static_assert(std::is_integral< T >::value, "atomic fetch_and is not supported with non integral types");

			return std::atomic_fetch_and_explicit(&m_pack.m_atomic, p_to_and, static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::fetch_and(T p_to_and, bc_memory_order p_order) noexcept
		{
			static_assert(!std::is_same< T, bool >::value, "atomic fetch_and is not supported with bool");
			static_assert(std::is_integral< T >::value, "atomic fetch_and is not supported with non integral types");

			return std::atomic_fetch_and_explicit(&m_pack.m_atomic, p_to_and, static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::fetch_or(T p_to_or) volatile noexcept
		{
			static_assert(!std::is_same< T, bool >::value, "atomic fetch_or is not supported with bool");
			static_assert(std::is_integral< T >::value, "atomic fetch_or is not supported with non integral types");

			return std::atomic_fetch_or(&m_pack.m_atomic, p_to_or);
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::fetch_or(T p_to_or) noexcept
		{
			static_assert(!std::is_same< T, bool >::value, "atomic fetch_or is not supported with bool");
			static_assert(std::is_integral< T >::value, "atomic fetch_or is not supported with non integral types");

			return std::atomic_fetch_or(&m_pack.m_atomic, p_to_or);
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::fetch_or(T p_to_or, bc_memory_order p_order) volatile noexcept
		{
			static_assert(!std::is_same< T, bool >::value, "atomic fetch_or is not supported with bool");
			static_assert(std::is_integral< T >::value, "atomic fetch_or is not supported with non integral types");

			return std::atomic_fetch_or_explicit(&m_pack.m_atomic, p_to_or, static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		T bc_platform_atomic< TPlatform, T >::fetch_or(T p_to_or, bc_memory_order p_order) noexcept
		{
			static_assert(!std::is_same< T, bool >::value, "atomic fetch_or is not supported with bool");
			static_assert(std::is_integral< T >::value, "atomic fetch_or is not supported with non integral types");

			return std::atomic_fetch_or_explicit(&m_pack.m_atomic, p_to_or, static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		bool bc_platform_atomic< TPlatform, T >::compare_exchange_strong(T* p_expected_value, T p_new_value) volatile noexcept
		{
			return m_pack.m_atomic.compare_exchange_strong(*p_expected_value, p_new_value);
		}

		template< bc_platform TPlatform, typename T >
		bool bc_platform_atomic< TPlatform, T >::compare_exchange_strong(T* p_expected_value, T p_new_value) noexcept
		{
			return m_pack.m_atomic.compare_exchange_strong(*p_expected_value, p_new_value);
		}

		template< bc_platform TPlatform, typename T >
		bool bc_platform_atomic< TPlatform, T >::compare_exchange_strong(T* p_expected_value, T p_new_value, bc_memory_order p_order) volatile noexcept
		{
			return m_pack.m_atomic.compare_exchange_strong(*p_expected_value, p_new_value, static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		bool bc_platform_atomic< TPlatform, T >::compare_exchange_strong(T* p_expected_value, T p_new_value, bc_memory_order p_order) noexcept
		{
			return m_pack.m_atomic.compare_exchange_strong(*p_expected_value, p_new_value, static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		bool bc_platform_atomic< TPlatform, T >::compare_exchange_strong(T* p_expected_value,
			T p_new_value,
			bc_memory_order p_success_order,
			bc_memory_order p_failure_order) volatile noexcept
		{
			return m_pack.m_atomic.compare_exchange_strong
			(
				*p_expected_value,
				p_new_value,
				static_cast<std::memory_order>(p_success_order),
				static_cast<std::memory_order>(p_failure_order)
			);
		}

		template< bc_platform TPlatform, typename T >
		bool bc_platform_atomic< TPlatform, T >::compare_exchange_strong(T* p_expected_value,
			T p_new_value,
			bc_memory_order p_success_order,
			bc_memory_order p_failure_order) noexcept
		{
			return m_pack.m_atomic.compare_exchange_strong
			(
				*p_expected_value,
				p_new_value,
				static_cast<std::memory_order>(p_success_order),
				static_cast<std::memory_order>(p_failure_order)
			);
		}

		template< bc_platform TPlatform, typename T >
		bool bc_platform_atomic< TPlatform, T >::compare_exchange_weak(T* p_expected_value, T p_new_value) volatile noexcept
		{
			return m_pack.m_atomic.compare_exchange_weak(*p_expected_value, p_new_value);
		}

		template< bc_platform TPlatform, typename T >
		bool bc_platform_atomic< TPlatform, T >::compare_exchange_weak(T* p_expected_value, T p_new_value) noexcept
		{
			return m_pack.m_atomic.compare_exchange_weak(*p_expected_value, p_new_value);
		}

		template< bc_platform TPlatform, typename T >
		bool bc_platform_atomic< TPlatform, T >::compare_exchange_weak(T* p_expected_value, T p_new_value, bc_memory_order p_order) volatile noexcept
		{
			return m_pack.m_atomic.compare_exchange_weak(*p_expected_value, p_new_value, static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		bool bc_platform_atomic< TPlatform, T >::compare_exchange_weak(T* p_expected_value, T p_new_value, bc_memory_order p_order) noexcept
		{
			return m_pack.m_atomic.compare_exchange_weak(*p_expected_value, p_new_value, static_cast< std::memory_order >(p_order));
		}

		template< bc_platform TPlatform, typename T >
		bool bc_platform_atomic< TPlatform, T >::compare_exchange_weak(T* p_expected_value,
			T p_new_value,
			bc_memory_order p_success_order,
			bc_memory_order p_failure_order) volatile noexcept
		{
			return m_pack.m_atomic.compare_exchange_weak
			(
				*p_expected_value,
				p_new_value,
				static_cast<std::memory_order>(p_success_order),
				static_cast<std::memory_order>(p_failure_order)
			);
		}

		template< bc_platform TPlatform, typename T >
		bool bc_platform_atomic< TPlatform, T >::compare_exchange_weak(T* p_expected_value,
			T p_new_value,
			bc_memory_order p_success_order,
			bc_memory_order p_failure_order) noexcept
		{
			return m_pack.m_atomic.compare_exchange_weak
			(
				*p_expected_value,
				p_new_value,
				static_cast<std::memory_order>(p_success_order),
				static_cast<std::memory_order>(p_failure_order)
			);
		}

		inline void atomic_thread_fence(bc_memory_order p_order) noexcept
		{
			std::atomic_thread_fence(static_cast<std::memory_order>(p_order));
		}
	}
}