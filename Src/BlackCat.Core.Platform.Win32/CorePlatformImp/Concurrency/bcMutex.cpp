// [28/10/2016 MRB]

#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/bcExport.h"
#include <array>

namespace black_cat::platform
{
	// provide definitions at first to prevent CLang 'explicit specialization of '' after instantiation' error

	template<>
	void bc_platform_hybrid_mutex<bc_platform::win32>::lock(bc_lock_operation p_lock_operation);

	template<>
	bool bc_platform_hybrid_mutex<bc_platform::win32>::try_lock(bc_lock_operation p_lock_operation) noexcept;

	template<>
	BC_COREPLATFORMIMP_DLL
	void bc_platform_hybrid_mutex<bc_platform::win32>::lock()
	{
		lock(bc_lock_operation::medium);
	}

	template<>
	BC_COREPLATFORMIMP_DLL
	void bc_platform_hybrid_mutex<bc_platform::win32>::lock(bc_lock_operation p_lock_operation)
	{
		const bcINT32 l_new_iteration_count = std::array<INT32, 3>{1000, 500, 100}[static_cast<bcINT32>(p_lock_operation)];
		bcINT32 l_expected = 0;
		bcINT32 l_expected_iteration = 0;
		bcINT32 l_iterator = 0;

#ifdef BC_DEBUG
		// prevent double lock on same thread
		BC_ASSERT(m_pack.m_thread_id.load(bc_memory_order::relaxed) != bc_thread::current_thread_id());
#endif

		while (true)
		{
			l_expected = 0;
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
			if (l_expected_iteration != l_expected)
			{
				l_iterator = l_expected;
				l_expected_iteration = l_expected;
			}

			--l_iterator;

			if (l_iterator % 5 == 0)
			{
				bc_thread::current_thread_yield();
				continue;
			}

			if(l_iterator == 0)
			{
				bc_thread::current_thread_yield_switch();
				l_iterator = l_expected_iteration;
			}
		}
	}

	template<>
	BC_COREPLATFORMIMP_DLL
	void bc_platform_hybrid_mutex<bc_platform::win32>::unlock() noexcept
	{
#ifdef BC_DEBUG
		// only those thread that acquired the lock can unlock it
		BC_ASSERT(m_pack.m_thread_id.load(bc_memory_order::relaxed) == bc_thread::current_thread_id());
		m_pack.m_thread_id.store(0U, bc_memory_order::relaxed);
#endif
		m_pack.m_flag.store(0, bc_memory_order::release);
	}

	template<>
	BC_COREPLATFORMIMP_DLL
	bool bc_platform_hybrid_mutex<bc_platform::win32>::try_lock() noexcept
	{
		return try_lock(bc_lock_operation::medium);
	}

	template<>
	BC_COREPLATFORMIMP_DLL
	bool bc_platform_hybrid_mutex<bc_platform::win32>::try_lock(bc_lock_operation p_lock_operation) noexcept
	{
		const bcINT32 l_new_iteration_count = static_cast<bcINT32>(p_lock_operation);
		bcINT32 l_expected = 0;
#ifdef BC_DEBUG
		// prevent double lock on same thread
		BC_ASSERT(m_pack.m_thread_id.load(bc_memory_order::relaxed) != bc_thread::current_thread_id());
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
