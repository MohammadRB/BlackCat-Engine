// [10/13/2014 MRB]

#pragma once

#include <thread>
#include "CorePlatform/bcCorePlatformUtility.h"
#include "CorePlatform/Concurrency/bcThread.h"
#include "CorePlatform/bcPlatform.h"
#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"

namespace black_cat
{
	namespace core_platform
	{
		template<>
		struct bc_thread_pack< bc_platform::win32 >
		{
			std::thread m_thread;

			bc_thread_pack()
			{
			}

			bc_thread_pack(std::thread&& p_thread)
				: m_thread(std::move(p_thread))
			{
			}

			template< typename Callable, typename ...Args >
			bc_thread_pack(Callable&& p_func, Args&&... p_args)
				: m_thread(std::forward< Callable >(p_func), std::forward< Args >(p_args)...)
			{
			}
		};

		template<>
		inline bc_thread_proxy< bc_platform::win32 >::bc_thread_proxy() noexcept(true)
		{
		};

		template<>
		inline bc_thread_proxy< bc_platform::win32 >::bc_thread_proxy(this_type&& p_other) noexcept(true)
			: m_pack(std::move(p_other.m_pack.m_thread))
		{
		};

		template<>
		template< typename Callable, typename ...Args >
		inline bc_thread_proxy< bc_platform::win32 >::bc_thread_proxy(Callable&& p_func, Args&&... p_args)
			: m_pack(std::forward< Callable >(p_func), std::forward< Args >(p_args)...)
		{
		}

		template< bc_platform TP >
		typename bc_thread_proxy< TP >::this_type& bc_thread_proxy< TP >::operator=(this_type&& p_other) noexcept(true)
		{
			m_pack.m_pack.m_thread = std::move(p_other.m_pack.m_pack.m_thread);

			return *this;
		};

		template<>
		inline void bc_thread_proxy< bc_platform::win32 >::swap(this_type& p_other) noexcept(true)
		{
			m_pack.m_thread.swap(p_other.m_pack.m_thread);
		};

		template<>
		inline void bc_thread_proxy< bc_platform::win32 >::join()
		{
			m_pack.m_thread.join();
		};

		template<>
		inline void bc_thread_proxy< bc_platform::win32 >::detach()
		{
			m_pack.m_thread.detach();
		};

		template<>
		inline bool bc_thread_proxy< bc_platform::win32 >::joinable() const noexcept(true)
		{
			return m_pack.m_thread.joinable();
		};

		template<>
		inline bc_thread_proxy<bc_platform::win32>::id bc_thread_proxy< bc_platform::win32 >::get_id() const noexcept(true)
		{
			return m_pack.m_thread.get_id().hash();
		};

		template<>
		inline bc_thread_proxy<bc_platform::win32>::id bc_thread_proxy< bc_platform::win32 >::current_thread_id() noexcept(true)
		{
			return std::this_thread::get_id().hash();
		};

		template<>
		inline void bc_thread_proxy< bc_platform::win32 >::current_thread_sleep_for(bcUINT64 p_nano)
		{
			std::this_thread::sleep_for(std::chrono::nanoseconds(p_nano));
		};

		template<>
		inline void bc_thread_proxy< bc_platform::win32 >::current_thread_yield() noexcept(true)
		{
			YieldProcessor();
		};

		template<>
		inline void bc_thread_proxy< bc_platform::win32 >::current_thread_yield_switch() noexcept(true)
		{
			SwitchToThread();
		};
	}
}