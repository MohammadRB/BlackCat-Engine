// [10/10/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/bcCorePlatformUtility.h"
#include "CorePlatform/Concurrency/bcConcurrencyDef.h"

namespace black_cat
{
	namespace core_platform
	{
		template< bc_platform TP, typename T >
		class bc_future_provider : private bc_no_copy
		{
		public:

		protected:

		private:

		};

		template< bc_platform TP, typename T >
		class bc_shared_future_provider
		{
		public:

		protected:

		private:

		};

		template< bc_platform TP, typename T >
		class bc_promise_provider : private bc_no_copy
		{
		public:

		protected:

		private:

		};

		template< class TProvider, typename T >
		class bc_shared_future_proxy;

		template< class TProvider, typename T >
		class bc_promise_proxy;

		template< class TProvider, typename T >
		class bc_future_proxy : private bc_no_copy
		{
			using type = T;
			using provider_type = TProvider;
			using this_type = bc_future_proxy< provider_type, T >;
			using shared_type = bc_shared_future_proxy< bc_shared_future_provider< g_current_platform, T >, T >;
			using promise_type = bc_promise_proxy< bc_promise_provider< g_current_platform, T >, T >;

			friend class promise_type;

		public:
			bc_future_proxy() noexcept(true)
				: m_provier()
			{
			}

			bc_future_proxy(this_type&& p_other) noexcept(true)
				: m_provier(std::move(p_other.m_provier))
			{
			}

			~bc_future_proxy() noexcept(true)
			{
			}

			this_type& operator =(this_type&& p_other) noexcept(true)
			{
				m_provier.operator =(std::forward(p_other.m_provier));

				return *this;
			}

			bcInline shared_type share()
			{
				shared_type l_temp(std::move(*this));

				return l_temp;
			}

			bcInline bool valid() const noexcept(true)
			{
				return m_provier.valid();
			}

			bcInline type get()
			{
				return m_provier.get();
			}

			bcInline void wait() const noexcept(true)
			{
				m_provier.wait();
			}

			bcInline bc_future_status wait_for(const bcUINT64 p_nano) const
			{
				return m_provier.wait_for(std::chrono::nanoseconds(p_nano));
			}

		protected:

		private:
			provider_type m_provier;

		};

		template< class TProvider, typename T >
		class bc_shared_future_proxy
		{
			using type = T;
			using provider_type = TProvider;
			using this_type = bc_shared_future_proxy< provider_type, T >;
			using future_type = bc_future_proxy< bc_future_provider< g_current_platform, T >, T >;

			friend class future_type;

		public:
			bc_shared_future_proxy() noexcept(true)
				: m_provider()
			{
			}

			bc_shared_future_proxy(const this_type& p_other) noexcept(true)
				: m_provider(p_other.m_provider)
			{
			}

			bc_shared_future_proxy(this_type&& p_other) noexcept(true)
				: m_provider(std::move(p_other.m_provider))
			{
			}

			bc_shared_future_proxy(future_type&& p_future)
				: m_provider(std::move(p_future.m_provider))
			{
			}

			~bc_shared_future_proxy() noexcept(true)
			{
			}

			this_type& operator =(const this_type& p_other)
			{
				m_provider = p_other.m_provider;

				return *this;
			}

			this_type& operator =(this_type&& p_other)
			{
				m_provider = std::move(p_other.m_provider);

				return *this;
			}

			bcInline bool valid() const noexcept(true)
			{
				return m_provider.valid();
			}

			bcInline type get() const
			{
				return m_provider.get();
			}

			bcInline void wait() const noexcept(true)
			{
				m_provider.wait();
			}

			bcInline bc_future_status wait_for(const bcUINT64 p_nano) const
			{
				return m_provider.wait_for(std::chrono::nanoseconds(p_nano));
			}

		protected:

		private:
			provider_type m_provider;

		};

		template< class TProvider, typename T >
		class bc_promise_proxy : private bc_no_copy
		{
			using type = T;
			using provider_type = TProvider;
			using this_type = bc_promise_proxy< provider_type, T >;
			using future_type = bc_future_proxy< bc_future_provider< g_current_platform, T >, T >;

		public:
			bc_promise_proxy() noexcept(true)
				: m_provider()
			{
			}

			// TODO: Check here
			/*template<typename Allocator>
			bcPromiseBase(std::allocator_arg_t, Allocator const&);*/

			bc_promise_proxy(this_type&& p_other) noexcept(true)
				: m_provider(std::move(p_other.m_provider))
			{
			}

			~bc_promise_proxy()
			{
			}

			this_type& operator =(this_type&& p_other)
			{
				m_provider.operator =(std::move(p_other.m_provider));

				return *this;
			}

			bcInline void swap(this_type& p_other) noexcept(true)
			{
				m_provider.swap(p_other.m_provider);
			}

			bcInline future_type get_future()
			{
				future_type lTemp;
				lTemp.m_future = std::move(m_provider.get_future());

				return lTemp;
			}

			bcInline void set_value(const type& p_value)
			{
				m_provider.set_value(p_value);
			}

			bcInline void set_value(type&& p_value)
			{
				m_provider.set_value(std::move(p_value));
			}

			bcInline void set_exception(std::exception_ptr p_exception)
			{
				m_provider.set_exception(p_exception);
			}

		protected:

		private:
			provider_type m_provider;

		};

		template< typename T >
		using bc_future = bc_future_proxy< bc_future_provider< g_current_platform, T >, T >;
		template< typename T >
		using bc_shared_future = bc_shared_future_proxy< bc_shared_future_provider< g_current_platform, T >, T >;
		template< typename T >
		using bc_promise = bc_promise_proxy< bc_promise_provider< g_current_platform, T >, T >;
	}
}