// [10/13/2014 MRB]

#pragma once

#include <thread>
#include "CorePlatform/Concurrency/bcThread.h"
#include "CorePlatform/bcPlatform.h"
#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "CorePlatformImp/bcUtility.h"

namespace black_cat
{
	namespace core_platform
	{
		template<>
		struct bc_platform_thread_pack< bc_platform::win32 >
		{
			bc_platform_thread_pack()
			{
			}

			bc_platform_thread_pack(std::thread&& p_thread)
				: m_thread(std::move(p_thread))
			{
			}

			template< typename Callable, typename ...Args >
			bc_platform_thread_pack(Callable&& p_func, Args&&... p_args)
				: m_thread(std::forward< Callable >(p_func), std::forward< Args >(p_args)...)
			{
			}

			std::thread m_thread;
		};

		template<>
		inline bc_platform_thread< bc_platform::win32 >::bc_platform_thread() noexcept
		{
		}

		template<>
		inline bc_platform_thread< bc_platform::win32 >::bc_platform_thread(this_type&& p_other) noexcept
			: m_pack(std::move(p_other.m_pack.m_thread))
		{
		}

		template<>
		template< typename Callable, typename ...Args >
		inline bc_platform_thread< bc_platform::win32 >::bc_platform_thread(Callable&& p_func, Args&&... p_args)
			: m_pack(std::forward< Callable >(p_func), std::forward< Args >(p_args)...)
		{
		}

		template<>
		inline bc_platform_thread< bc_platform::win32 >::~bc_platform_thread()
		{
		}

		template<>
		inline bc_platform_thread< bc_platform::win32 >::this_type& bc_platform_thread< bc_platform::win32 >::operator=(this_type&& p_other) noexcept
		{
			m_pack.m_thread = std::move(p_other.m_pack.m_thread);

			return *this;
		}

		template<>
		inline void bc_platform_thread< bc_platform::win32 >::swap(this_type& p_other) noexcept
		{
			m_pack.m_thread.swap(p_other.m_pack.m_thread);
		}

		template<>
		inline void bc_platform_thread< bc_platform::win32 >::join()
		{
			m_pack.m_thread.join();
		}

		template<>
		inline void bc_platform_thread< bc_platform::win32 >::detach()
		{
			m_pack.m_thread.detach();
		}

		template<>
		inline bool bc_platform_thread< bc_platform::win32 >::joinable() const noexcept
		{
			return m_pack.m_thread.joinable();
		}

		template<>
		inline bc_platform_thread<bc_platform::win32>::id bc_platform_thread< bc_platform::win32 >::get_id() const noexcept
		{
			return std::hash< std::thread::id >()(m_pack.m_thread.get_id());
		}

		template<>
		inline const bcWCHAR* bc_platform_thread<bc_platform::win32>::get_name() const noexcept
		{
			PWSTR l_desc;
			win_call(GetThreadDescription(const_cast<std::thread&>(m_pack.m_thread).native_handle(), &l_desc));

			return l_desc;
		}

		template<>
		inline void bc_platform_thread<bc_platform::win32>::set_name(const bcWCHAR* p_name) noexcept
		{
			win_call(SetThreadDescription(m_pack.m_thread.native_handle(), p_name));
		}

		template<>
		inline bc_thread_priority bc_platform_thread<bc_platform::win32>::get_priority() const noexcept
		{
			auto l_priority = bc_thread_priority::normal;
			const auto l_priority_int = GetThreadPriority(const_cast<std::thread&>(m_pack.m_thread).native_handle());
			
			switch (l_priority_int)
			{
			case THREAD_PRIORITY_LOWEST:
				l_priority = bc_thread_priority::lowest;
				break;
			case THREAD_PRIORITY_BELOW_NORMAL:
				l_priority = bc_thread_priority::below_normal;
				break;
			case THREAD_PRIORITY_NORMAL:
				l_priority = bc_thread_priority::normal;
				break;
			case THREAD_PRIORITY_ABOVE_NORMAL:
				l_priority = bc_thread_priority::above_normal;
				break;
			case THREAD_PRIORITY_HIGHEST:
				l_priority = bc_thread_priority::highest;
				break;
			default:
				BC_ASSERT(false);
			}

			return l_priority;
		}

		template<>
		inline void bc_platform_thread<bc_platform::win32>::set_priority(bc_thread_priority p_priority) noexcept
		{
			auto l_priority = THREAD_PRIORITY_NORMAL;
			
			switch (p_priority)
			{
			case bc_thread_priority::lowest:
				l_priority = THREAD_PRIORITY_LOWEST;
				break;
			case bc_thread_priority::below_normal:
				l_priority = THREAD_PRIORITY_BELOW_NORMAL;
				break;
			case bc_thread_priority::normal:
				l_priority = THREAD_PRIORITY_NORMAL;
				break;
			case bc_thread_priority::above_normal:
				l_priority = THREAD_PRIORITY_ABOVE_NORMAL;
				break;
			case bc_thread_priority::highest:
				l_priority = THREAD_PRIORITY_HIGHEST;
				break;
			default: 
				BC_ASSERT(false);
			}
			
			win_call(static_cast<bool>(SetThreadPriority(m_pack.m_thread.native_handle(), l_priority)));
		}

		template<>
		inline bc_platform_thread<bc_platform::win32>::id bc_platform_thread< bc_platform::win32 >::current_thread_id() noexcept
		{
			return std::hash<std::thread::id>()(std::this_thread::get_id());
		}

		template<>
		inline void bc_platform_thread< bc_platform::win32 >::current_thread_sleep_for(const std::chrono::nanoseconds& p_duration)
		{
			std::this_thread::sleep_for(p_duration);
		}

		template<>
		inline void bc_platform_thread< bc_platform::win32 >::current_thread_sleep_for(const std::chrono::microseconds& p_duration)
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
		inline void bc_platform_thread< bc_platform::win32 >::current_thread_yield() noexcept
		{
			YieldProcessor();
		}

		template<>
		inline void bc_platform_thread< bc_platform::win32 >::current_thread_yield_switch() noexcept
		{
			//SwitchToThread();
			Sleep(0);
		}
	}
}