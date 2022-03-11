// [10/12/2014 MRB]

#pragma once

#include <condition_variable>
#include "CorePlatform/Utility/bcNoCopy.h"
#include "CorePlatform/Concurrency/bcConditionVariable.h"
#include "CorePlatformImp/CorePlatformImpPCH.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_condition_variable_pack<bc_platform::win32>
		{
			std::condition_variable_any m_con_var;
		};

		template<>
		inline bc_platform_condition_variable<bc_platform::win32>::bc_platform_condition_variable()
		{
		};

		template<>
		inline bc_platform_condition_variable<bc_platform::win32>::~bc_platform_condition_variable()
		{
		};

		template<>
		inline void bc_platform_condition_variable<bc_platform::win32>::notify_one()
		{
			m_pack.m_con_var.notify_one();
		};

		template<>
		inline void bc_platform_condition_variable<bc_platform::win32>::notify_all() noexcept(true)
		{
			m_pack.m_con_var.notify_all();
		};

		template<>
		template< typename Lockable >
		inline void bc_platform_condition_variable<bc_platform::win32>::wait(Lockable& p_lock)
		{
			m_pack.m_con_var.wait(p_lock);
		};

		template<>
		template< typename Lockable, typename Predicate >
		inline void bc_platform_condition_variable<bc_platform::win32>::wait(Lockable& p_lock, Predicate p_pred)
		{
			m_pack.m_con_var.wait(p_lock, p_pred);
		};

		template<>
		template< typename Lockable >
		inline bool bc_platform_condition_variable<bc_platform::win32>::wait_for(Lockable& p_lock, const bcUINT64 p_nano)
		{
			return (m_pack.m_con_var.wait_for(p_lock, std::chrono::nanoseconds(p_nano)));
		}

		template<>
		template< typename Lockable, typename Predicate >
		inline bool bc_platform_condition_variable<bc_platform::win32>::wait_for(Lockable& p_lock, const bcUINT64 p_nano, Predicate p_pred)
		{
			return m_pack.m_con_var.wait_for(p_lock, std::chrono::nanoseconds(p_nano), p_pred);
		};
	}
}