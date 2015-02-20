// [10/12/2014 MRB]

#pragma once

#include <condition_variable>
#include "CorePlatform/bcCorePlatformUtility.h"
#include "CorePlatform/Concurrency/bcConditionVariableProvider.h"
#include "CorePlatformImp/CoreWin32PCH.h"

namespace black_cat
{
	namespace core_platform
	{
		template<>
		class bc_condition_variable_provider< bc_platform::win32 > : public bc_condition_variable_provider< bc_platform::unknown >
		{
		public:
			bc_condition_variable_provider() 
				: m_con_var()
			{
			}

			~bc_condition_variable_provider()
			{
			}

			bcInline void notify_one() noexcept(true)
			{
				m_con_var.notify_one(); 
			}

			bcInline void notify_all() noexcept(true)
			{ 
				m_con_var.notify_all(); 
			}

			template< typename Lockable >
			bcInline void wait(Lockable& p_lock)
			{
				m_con_var.wait(p_lock);
			}

			template < typename Lockable, typename Predicate >
			bcInline void wait(Lockable& p_lock, Predicate p_pred)
			{
				m_con_var.wait(p_lock, p_pred);
			}

			template < typename Lockable >
			bcInline bool wait_for(Lockable& p_lock, const bcUINT64 p_nano)
			{
				return (m_con_var.wait_for(p_lock, std::chrono::nanoseconds(p_nano)));
			}

			template < typename Lockable, typename Predicate >
			bcInline bool wait_for(Lockable& p_lock, const bcUINT64 p_nano, Predicate p_pred)
			{
				return m_con_var.wait_for(p_lock, std::chrono::nanoseconds(p_nano), p_pred);
			}

		protected:

		private:
			std::condition_variable_any m_con_var;

		};
	}
}