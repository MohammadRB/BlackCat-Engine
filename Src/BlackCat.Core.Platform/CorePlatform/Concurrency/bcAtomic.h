// [10/13/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/Concurrency/bcConcurrencyDef.h"
#include "CorePlatform/Utility/bcNoCopy.h"

namespace black_cat
{
	namespace core_platform
	{
		template< bc_platform TPlatform >
		struct bc_platform_atomic_flag_pack {};

		template< bc_platform TPlatform, typename T >
		struct bc_platform_atomic_pack {};

		template< bc_platform TPlatform >
		class bc_platform_atomic_flag : private bc_no_copy
		{
		public:
			using platform_pack = bc_platform_atomic_flag_pack< TPlatform >;
			using this_type = bc_platform_atomic_flag;

		public:
			bc_platform_atomic_flag() noexcept;

			bc_platform_atomic_flag(bool p_flag) noexcept;

			~bc_platform_atomic_flag() noexcept;

			bool test_and_set() volatile noexcept;

			bool test_and_set() noexcept;

			bool test_and_set(bc_memory_order p_order) volatile noexcept;

			bool test_and_set(bc_memory_order p_order) noexcept;

			void clear() volatile noexcept;

			void clear() noexcept;

			void clear(bc_memory_order p_order) volatile noexcept;

			void clear(bc_memory_order p_order) noexcept;

		private:
			platform_pack m_pack;
		};

		template< bc_platform TPlatform, typename T >
		class bc_platform_atomic : private bc_no_copy
		{
		public:
			using platform_pack = bc_platform_atomic_pack< TPlatform, T >;
			using this_type = bc_platform_atomic;

		public:
			bc_platform_atomic() noexcept;

			~bc_platform_atomic() noexcept;

			bc_platform_atomic(T p_value) noexcept;

			bool is_lock_free() const volatile noexcept;

			bool is_lock_free() const noexcept;

			T exchange(T p_value) volatile noexcept;

			T exchange(T p_value) noexcept;

			T exchange(T p_value, bc_memory_order p_order) volatile noexcept;

			T exchange(T p_value, bc_memory_order p_order) noexcept;

			void store(T p_value) volatile noexcept;

			void store(T p_value) noexcept;

			void store(T p_value, bc_memory_order p_order) volatile noexcept;

			void store(T p_value, bc_memory_order p_order) noexcept;

			T load() const volatile noexcept;

			T load() const noexcept;

			T load(bc_memory_order p_order) const volatile noexcept;

			T load(bc_memory_order p_order) const noexcept;

			T fetch_add(T p_to_add) volatile noexcept;

			T fetch_add(T p_to_add) noexcept;

			T fetch_add(T p_to_add, bc_memory_order p_order) volatile noexcept;

			T fetch_add(T p_to_add, bc_memory_order p_order) noexcept;

			T fetch_sub(T p_to_sub) volatile noexcept;

			T fetch_sub(T p_to_sub) noexcept;

			T fetch_sub(T p_to_sub, bc_memory_order p_order) volatile noexcept;

			T fetch_sub(T p_to_sub, bc_memory_order p_order) noexcept;

			T fetch_and(T p_to_and) volatile noexcept;

			T fetch_and(T p_to_and) noexcept;

			T fetch_and(T p_to_and, bc_memory_order p_order) volatile noexcept;

			T fetch_and(T p_to_and, bc_memory_order p_order) noexcept;

			T fetch_or(T p_to_or) volatile noexcept;

			T fetch_or(T p_to_or) noexcept;

			T fetch_or(T p_to_or, bc_memory_order p_order) volatile noexcept;

			T fetch_or(T p_to_or, bc_memory_order p_order) noexcept;

			bool compare_exchange_strong(T* p_expected_value, T p_new_value) volatile noexcept;

			bool compare_exchange_strong(T* p_expected_value, T p_new_value) noexcept;

			bool compare_exchange_strong(T* p_expected_value, T p_new_value, bc_memory_order p_order) volatile noexcept;

			bool compare_exchange_strong(T* p_expected_value, T p_new_value, bc_memory_order p_order) noexcept;

			bool compare_exchange_strong(T* p_expected_value,
				T p_new_value,
				bc_memory_order p_success_order,
				bc_memory_order p_failure_order) volatile noexcept;

			bool compare_exchange_strong(T* p_expected_value,
				T p_new_value,
				bc_memory_order p_success_order,
				bc_memory_order p_failure_order) noexcept;

			bool compare_exchange_weak(T* p_expected_value, T p_new_value) volatile noexcept;

			bool compare_exchange_weak(T* p_expected_value, T p_new_value) noexcept;

			bool compare_exchange_weak(T* p_expected_value, T p_new_value, bc_memory_order p_order) volatile noexcept;

			bool compare_exchange_weak(T* p_expected_value, T p_new_value, bc_memory_order p_order) noexcept;

			bool compare_exchange_weak(T* p_expected_value,
				T p_new_value,
				bc_memory_order p_success_order,
				bc_memory_order p_failure_order) volatile noexcept;

			bool compare_exchange_weak(T* p_expected_value,
				T p_new_value,
				bc_memory_order p_success_order,
				bc_memory_order p_failure_order) noexcept;

		private:
			platform_pack m_pack;
		};

		using bc_atomic_flag = bc_platform_atomic_flag< g_current_platform >;

		template< typename T >
		using bc_atomic = bc_platform_atomic< g_current_platform, T >;
	}
}