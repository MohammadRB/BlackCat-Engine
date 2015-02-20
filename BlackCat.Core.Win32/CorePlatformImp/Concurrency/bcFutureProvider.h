// [10/11/2014 MRB]

#pragma once

#include <future>
#include "CorePlatform/bcCorePlatformUtility.h"
#include "CorePlatform/Concurrency/bcFutureProvider.h"
#include "CorePlatformImp/CoreWin32PCH.h"

namespace black_cat
{
	namespace core_platform
	{
		// TODO Write bcFutureProvider, bcSharedFutureProvider, bcPromiseProvider T& and void specialization /

		template< typename T >
		class bc_shared_future_provider< bc_platform::win32, T>;

		template< typename T >
		class bc_promise_provider< bc_platform::win32, T >;

		template< typename T >
		class bc_future_provider< bc_platform::win32, T > : public bc_future_provider< bc_platform::unknown, T >
		{
			typedef bc_future_provider< bc_platform::win32, T > this_type;
			typedef bc_shared_future_provider< bc_platform::win32, T> shared_type;
			typedef bc_promise_provider< bc_platform::win32, T > promise_type;

			friend class promise_type;

		public:
			bc_future_provider() noexcept(true) 
				: m_future()
			{
			}

			bc_future_provider(this_type&& p_other) noexcept(true)
				: m_future(std::move(p_other.m_future))
			{ 
			}

			~bc_future_provider() noexcept(true) 
			{
			}

			this_type& operator =(this_type&& p_other) noexcept(true)
			{
				m_future.operator =(std::forward(p_other.m_future));

				return *this;
			}

			bcInline shared_type share()
			{
				shared_type lTemp(std::move(*this));

				return lTemp;
			}

			bcInline bool valid() const noexcept(true)
			{
				return m_future.valid();
			}

			bcInline T get()
			{
				return m_future.get();
			}

			bcInline void wait() const noexcept(true)
			{
				m_future.wait();
			}

			bcInline bc_future_status wait_for(const bcUINT64 p_nano) const
			{
				return static_cast<bc_future_status>(m_future.wait_for(std::chrono::nanoseconds(p_nano)));
			}

		protected:

		private:
			std::future<T> m_future;

		};

		template< typename T >
		class bc_shared_future_provider< bc_platform::win32, T > : public bc_shared_future_provider< bc_platform::unknown, T >
		{
			typedef bc_shared_future_provider< bc_platform::win32, T > this_type;
			typedef bc_future_provider< bc_platform::win32, T > future_type;

			friend class future_type;

		public:
			bc_shared_future_provider() noexcept(true)
				: m_future()
			{
			}

			bc_shared_future_provider(const this_type& p_other) noexcept(true)
				: m_future(p_other.m_future)
			{
			}

			bc_shared_future_provider(this_type&& p_other) noexcept(true)
				: m_future(std::move(p_other.m_future))
			{
			}

			bc_shared_future_provider(future_type&& m_future)
				: m_future(std::move(m_future.m_future))
			{
			}

			~bc_shared_future_provider() noexcept(true)
			{
			}

			this_type& operator =(const this_type& p_other)
			{
				m_future = p_other.m_future;

				return *this;
			}

			this_type& operator =(this_type&& p_other)
			{
				m_future = std::move(p_other.m_future);

				return *this;
			}

			bcInline bool valid() const noexcept(true)
			{
				return m_future.valid();
			}

			bcInline T get() const
			{
				return m_future.get();
			}

			bcInline void wait() const noexcept(true)
			{
				m_future.wait();
			}

			bcInline bc_future_status wait_for(const bcUINT64 p_nano) const
			{
				return static_cast<bc_future_status>(m_future.wait_for(std::chrono::nanoseconds(p_nano)));
			}

		protected:

		private:
			std::shared_future< T > m_future;

		};

		template< typename T >
		class bc_promise_provider< bc_platform::win32, T > : public bc_promise_provider< bc_platform::unknown, T >
		{
			typedef bc_promise_provider< bc_platform::win32, T > this_type;
			typedef bc_future_provider< bc_platform::win32, T > future_type;

		public:
			bc_promise_provider() noexcept(true)
				: m_promise()
			{
			}
			
			// TODO: Check here
			/*template<typename Allocator>
			bcPromiseBase(std::allocator_arg_t, Allocator const&);*/

			bc_promise_provider(this_type&& p_other) noexcept(true)
				: m_promise(std::move(p_other.m_promise))
			{
			}

			~bc_promise_provider()
			{
			}

			this_type& operator =(this_type&& p_other)
			{
				m_promise.operator =(std::move(p_other.m_promise));

				return *this;
			}

			bcInline void swap(this_type& p_other) noexcept(true)
			{
				m_promise.swap(p_other.m_promise);
			}

			bcInline future_type get_future()
			{
				future_type lTemp;
				lTemp.m_future = std::move(m_promise.get_future());

				return lTemp;
			}

			bcInline void set_value(const T& p_value)
			{
				m_promise.set_value(p_value);
			}

			bcInline void set_value(T&& p_value)
			{
				m_promise.set_value(p_value);
			}
			
			bcInline void set_exception(std::exception_ptr p_exception)
			{
				m_promise.set_exception(p_exception);
			}

		protected:

		private:
			std::promise< T > m_promise;

		};
	}
}