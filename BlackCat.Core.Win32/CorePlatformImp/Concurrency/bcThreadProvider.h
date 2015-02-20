// [10/13/2014 MRB]

#pragma once

#include <thread>
#include "CorePlatform/bcCorePlatformUtility.h"
#include "CorePlatform/Concurrency/bcThreadProvider.h"
#include "CorePlatform/bcPlatform.h"
#include "CorePlatformImp/CoreWin32PCH.h"
#include "CorePlatformImp/Concurrency/bcAtomicProvider.h"

namespace black_cat
{
	namespace core_platform
	{
		template<>
		class bc_thread_provider< bc_platform::win32 > : public bc_thread_provider< bc_platform::unknown >
		{
			typedef bc_thread_provider< bc_platform::win32 > this_type;

		public:
			bc_thread_provider() noexcept(true)
				: m_thread()
			{
			}

			bc_thread_provider(this_type&& p_other) noexcept(true)
				: m_thread(std::move(p_other.m_thread))
			{
			}

			template< typename Callable, typename ...Args >
			explicit bc_thread_provider(Callable&& pFunc, Args&&... pArgs) 
				: m_thread(std::forward<Callable>(pFunc), std::forward<Args>(pArgs)...) 
			{
			}

			this_type& operator =(this_type&& p_other) noexcept(true)
			{
				m_thread.operator =(std::move(p_other.m_thread));

				return *this;
			}

			bcInline void swap(this_type& p_other) noexcept(true)
			{
				m_thread.swap(p_other.m_thread);
			}

			bcInline void join()
			{
				m_thread.join();
			}

			bcInline void detach()
			{
				m_thread.detach();
			}

			bcInline bool joinable() const noexcept(true)
			{
				return m_thread.joinable();
			}

			bcInline bcUINT32 get_id() const noexcept(true)
			{
				return m_thread.get_id().hash();
			}

			/*bcInline static bcUINT32 hardware_concurrency() noexcept(true) 
			{ 
				return std::thread::hardware_concurrency(); 
			}*/

			bcInline static bcUINT32 current_thread_id() noexcept(true)
			{ 
				return std::this_thread::get_id().hash(); 
			}

			bcInline static void current_thread_sleep_for(bcUINT64 p_nano) 
			{ 
				std::this_thread::sleep_for(std::chrono::nanoseconds(p_nano)); 
			}
			
			bcInline static void current_thread_yield() noexcept(true)
			{
				//std::this_thread::yield(); 
				YieldProcessor();
			}
			
			bcInline static void current_thread_yield_switch() noexcept(true)
			{
				//std::this_thread::yield();
				SwitchToThread();
			}

		protected:

		private:
			std::thread m_thread;

		};
	}
}