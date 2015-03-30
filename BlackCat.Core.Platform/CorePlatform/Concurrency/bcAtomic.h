// [10/13/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/bcCorePlatformUtility.h"

namespace black_cat
{
	namespace core_platform
	{
		template< bc_platform TPlatform >
		struct bc_atomic_flag_pack {};

		template< bc_platform TPlatform, typename T >
		struct bc_atomic_pack {};

		template< bc_platform TPlatform >
		class bc_atomic_flag_proxy : private bc_no_copy
		{
		public:
			using platform_pack = bc_atomic_flag_pack< TPlatform >;
			using this_type = bc_atomic_flag_proxy;

		public:
			bc_atomic_flag_proxy() noexcept(true);

			bc_atomic_flag_proxy(bool p_flag) noexcept(true);

			~bc_atomic_flag_proxy() noexcept(true);

			bcInline bool test_and_set() volatile noexcept(true);

			bcInline bool test_and_set() noexcept(true);

			bcInline bool test_and_set(bc_memory_order p_order) volatile noexcept(true);

			bcInline bool test_and_set(bc_memory_order p_order) noexcept(true);

			bcInline void clear() volatile noexcept(true);

			bcInline void clear() noexcept(true);

			bcInline void clear(bc_memory_order p_order) volatile noexcept(true);

			bcInline void clear(bc_memory_order p_order) noexcept(true);

		protected:

		private:
			platform_pack m_pack;
		};

		template< bc_platform TPlatform, typename T >
		class bc_atomic_proxy : private bc_no_copy
		{
		public:
			using platform_pack = bc_atomic_pack< TPlatform, T >;
			using this_type = bc_atomic_proxy;

		public:
			bc_atomic_proxy() noexcept(true);

			~bc_atomic_proxy() noexcept(true);

			bc_atomic_proxy(T p_value) noexcept(true);

			bcInline bool is_lock_free() const volatile noexcept(true);

			bcInline bool is_lock_free() const noexcept(true);

			bcInline T exchange(T p_value) volatile noexcept(true);

			bcInline T exchange(T p_value) noexcept(true);

			bcInline T exchange(T p_value, bc_memory_order p_order) volatile noexcept(true);

			bcInline T exchange(T p_value, bc_memory_order p_order) noexcept(true);

			bcInline void store(T p_value) volatile noexcept(true);

			bcInline void store(T p_value) noexcept(true);

			bcInline void store(T p_value, bc_memory_order p_order) volatile noexcept(true);

			bcInline void store(T p_value, bc_memory_order p_order) noexcept(true);

			bcInline T load() const volatile noexcept(true);

			bcInline T load() const noexcept(true);

			bcInline T load(bc_memory_order p_order) const volatile noexcept(true);

			bcInline T load(bc_memory_order p_order) const noexcept(true);

			bcInline T fetch_add(T p_to_add) volatile noexcept(true);

			bcInline T fetch_add(T p_to_add) noexcept(true);

			bcInline T fetch_add(T p_to_add, bc_memory_order p_order) volatile noexcept(true);

			bcInline T fetch_add(T p_to_add, bc_memory_order p_order) noexcept(true);

			bcInline T fetch_sub(T p_to_sub) volatile noexcept(true);

			bcInline T fetch_sub(T p_to_sub) noexcept(true);

			bcInline T fetch_sub(T p_to_sub, bc_memory_order p_order) volatile noexcept(true);

			bcInline T fetch_sub(T p_to_sub, bc_memory_order p_order) noexcept(true);

			bcInline T fetch_and(T p_to_and) volatile noexcept(true);

			bcInline T fetch_and(T p_to_and) noexcept(true);

			bcInline T fetch_and(T p_to_and, bc_memory_order p_order) volatile noexcept(true);

			bcInline T fetch_and(T p_to_and, bc_memory_order p_order) noexcept(true);

			bcInline T fetch_or(T p_to_or) volatile noexcept(true);

			bcInline T fetch_or(T p_to_or) noexcept(true);

			bcInline T fetch_or(T p_to_or, bc_memory_order p_order) volatile noexcept(true);

			bcInline T fetch_or(T p_to_or, bc_memory_order p_order) noexcept(true);

			bcInline bool compare_exchange_strong(T* p_expected_value, T p_new_value) volatile noexcept(true);

			bcInline bool compare_exchange_strong(T* p_expected_value, T p_new_value) noexcept(true);

			bcInline bool compare_exchange_strong(T* p_expected_value, T p_new_value, bc_memory_order p_order) volatile noexcept(true);

			bcInline bool compare_exchange_strong(T* p_expected_value, T p_new_value, bc_memory_order p_order) noexcept(true);

			bcInline bool compare_exchange_strong(T* p_expected_value,
				T p_new_value,
				bc_memory_order p_success_order,
				bc_memory_order p_failure_order) volatile noexcept(true);

			bcInline bool compare_exchange_strong(T* p_expected_value,
				T p_new_value,
				bc_memory_order p_success_order,
				bc_memory_order p_failure_order) noexcept(true);

			bcInline bool compare_exchange_weak(T* p_expected_value, T p_new_value) volatile noexcept(true);

			bcInline bool compare_exchange_weak(T* p_expected_value, T p_new_value) noexcept(true);

			bcInline bool compare_exchange_weak(T* p_expected_value, T p_new_value, bc_memory_order p_order) volatile noexcept(true);

			bcInline bool compare_exchange_weak(T* p_expected_value, T p_new_value, bc_memory_order p_order) noexcept(true);

			bcInline bool compare_exchange_weak(T* p_expected_value,
				T p_new_value,
				bc_memory_order p_success_order,
				bc_memory_order p_failure_order) volatile noexcept(true);

			bcInline bool compare_exchange_weak(T* p_expected_value,
				T p_new_value,
				bc_memory_order p_success_order,
				bc_memory_order p_failure_order) noexcept(true);

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_atomic_flag = bc_atomic_flag_proxy< g_current_platform >;

		template< typename T >
		using bc_atomic = bc_atomic_proxy< g_current_platform, T >;
	}
}