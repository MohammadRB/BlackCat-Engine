// [02/11/2020 MRB]

#pragma once

#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcThread.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"

namespace black_cat
{
	namespace core
	{
		class bc_mutex_test
		{
		private:
			using id_hasher = std::hash<platform::bc_thread::id>;
			using id_hash_t = bcSIZE;
			
		public:
			bc_mutex_test();

			bc_mutex_test(bc_mutex_test&&) = delete;

			~bc_mutex_test() = default;

			bc_mutex_test& operator=(bc_mutex_test&&) = delete;
			
			void lock();

			void unlock() noexcept;

			bool try_lock() noexcept;

		private:
			platform::bc_atomic<id_hash_t> m_thread_id;
		};

		using bc_mutex_test_guard = platform::bc_lock_guard<bc_mutex_test>;

		inline bc_mutex_test::bc_mutex_test()
			: m_thread_id(0)
		{
		}
		
		inline void bc_mutex_test::lock()
		{
#ifdef BC_DEBUG
			const auto l_current_thread_id = id_hasher()(platform::bc_thread::current_thread_id());
			auto l_expected = static_cast<id_hash_t>(0);

			const auto l_locked = m_thread_id.compare_exchange_strong
			(
				&l_expected,
				l_current_thread_id,
				platform::bc_memory_order::relaxed,
				platform::bc_memory_order::relaxed
			);

			BC_ASSERT(l_locked);
#endif
		}

		inline void bc_mutex_test::unlock() noexcept
		{
#ifdef BC_DEBUG
			const auto l_current_thread_id = id_hasher()(platform::bc_thread::current_thread_id());
			const auto l_thread_id = m_thread_id.load(platform::bc_memory_order::relaxed);
			
			BC_ASSERT(l_current_thread_id == l_thread_id);
			
			m_thread_id.store(static_cast< id_hash_t >(0), platform::bc_memory_order::relaxed);
#endif
		}

		inline bool bc_mutex_test::try_lock() noexcept
		{
			lock();
			return true;
		}
	}
}