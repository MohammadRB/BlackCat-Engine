// [10/13/2014 MRB]

#pragma once

#include <thread>
#include "CorePlatform/Concurrency/bcThread.h"
#include "CorePlatform/bcPlatform.h"
#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "CorePlatformImp/bcUtility.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_thread_pack<bc_platform::win32>
		{
			bc_platform_thread_pack() = default;

			bc_platform_thread_pack(std::thread&& p_thread)
				: m_thread(std::move(p_thread))
			{
			}

			template<typename Callable, typename ...Args>
			bc_platform_thread_pack(Callable&& p_func, Args&&... p_args)
				: m_thread(std::forward<Callable>(p_func), std::forward<Args>(p_args)...)
			{
			}

			std::thread m_thread;
		};

		template<>
		inline bc_platform_thread<bc_platform::win32>::bc_platform_thread() noexcept
		{
		}

		template<>
		inline bc_platform_thread<bc_platform::win32>::bc_platform_thread(this_type&& p_other) noexcept
			: m_pack(std::move(p_other.m_pack.m_thread))
		{
		}

		template<>
		template<typename Callable, typename ...Args>
		inline bc_platform_thread<bc_platform::win32>::bc_platform_thread(Callable&& p_func, Args&&... p_args)
			: m_pack(std::forward<Callable>(p_func), std::forward<Args>(p_args)...)
		{
		}

		template<>
		inline bc_platform_thread<bc_platform::win32>::~bc_platform_thread()
		{
		}

		template<>
		inline bc_platform_thread<bc_platform::win32>::this_type& bc_platform_thread<bc_platform::win32>::operator=(this_type&& p_other) noexcept
		{
			m_pack.m_thread = std::move(p_other.m_pack.m_thread);

			return *this;
		}

		template<>
		inline void bc_platform_thread<bc_platform::win32>::swap(this_type& p_other) noexcept
		{
			m_pack.m_thread.swap(p_other.m_pack.m_thread);
		}

		template<>
		inline void bc_platform_thread<bc_platform::win32>::join()
		{
			m_pack.m_thread.join();
		}

		template<>
		inline void bc_platform_thread<bc_platform::win32>::detach()
		{
			m_pack.m_thread.detach();
		}

		template<>
		inline bool bc_platform_thread<bc_platform::win32>::joinable() const noexcept
		{
			return m_pack.m_thread.joinable();
		}

		template<>
		inline bc_platform_thread<bc_platform::win32>::id bc_platform_thread<bc_platform::win32>::get_id() const noexcept
		{
			return GetThreadId(const_cast<std::thread&>(m_pack.m_thread).native_handle());
		}

		template<>
		inline bc_platform_thread<bc_platform::win32>::id bc_platform_thread<bc_platform::win32>::current_thread_id() noexcept
		{
			return GetCurrentThreadId();
		}

		template<>
		inline void bc_platform_thread<bc_platform::win32>::current_thread_sleep_for(const std::chrono::nanoseconds& p_duration)
		{
			std::this_thread::sleep_for(p_duration);
		}

		template<>
		inline void bc_platform_thread<bc_platform::win32>::current_thread_sleep_for(const std::chrono::microseconds& p_duration)
		{
			std::this_thread::sleep_for(p_duration);
		}

		template<>
		inline void bc_platform_thread<bc_platform::win32>::current_thread_sleep_for(const std::chrono::milliseconds& p_duration)
		{
			std::this_thread::sleep_for(p_duration);
		}

		template<>
		inline void bc_platform_thread<bc_platform::win32>::current_thread_sleep_for(const std::chrono::seconds& p_duration)
		{
			std::this_thread::sleep_for(p_duration);
		}

		template<>
		inline void bc_platform_thread<bc_platform::win32>::current_thread_yield() noexcept
		{
			YieldProcessor();
		}

		template<>
		inline void bc_platform_thread<bc_platform::win32>::current_thread_yield_switch() noexcept
		{
			//SwitchToThread();
			Sleep(0);
		}
	}
}