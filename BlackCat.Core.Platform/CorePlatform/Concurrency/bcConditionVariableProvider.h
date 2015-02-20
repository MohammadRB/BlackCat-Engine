// [10/11/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/bcPlatform.h"

namespace black_cat
{
	namespace core_platform
	{
		template< bc_platform TP >
		class bc_condition_variable_provider : private bc_no_copy
		{
		public:

		protected:

		private:

		};

		template< class TProvider >
		class bc_condition_variable_proxy
		{
			using provider_type = TProvider;
		public:
			bc_condition_variable_proxy() = default;

			~bc_condition_variable_proxy() = default;

			bcInline void notify_one()
			{
				m_provider.notify_one();
			}

			bcInline void notify_all() noexcept(true)
			{
				m_provider.notify_all();
			}

			template< typename Lockable >
			bcInline void wait(Lockable& p_lock)
			{
				m_provider.wait(p_lock);
			}

			template < typename Lockable, typename Predicate >
			bcInline void wait(Lockable& p_lock, Predicate p_pred)
			{
				m_provider.wait(p_lock, p_pred);
			}

			template < typename Lockable >
			bcInline bool wait_for(Lockable& p_lock, const bcUINT64 p_nano)
			{
				return m_provider.wait_for(p_lock, p_nano);
			}

			template < typename Lockable, typename Predicate >
			bcInline bool wait_for(Lockable& p_lock, const bcUINT64 p_nano, Predicate p_pred)
			{
				return m_provider.wait_for(p_lock, p_nano, p_pred);
			}

		protected:

		private:
			provider_type m_provider;
		};

		using bc_condition_variable = bc_condition_variable_proxy< bc_condition_variable_provider< g_current_platform > >;
	}
}