// [10/11/2014 MRB]

#pragma once

#include <future>
#include "CorePlatform/bcCorePlatformUtility.h"
#include "CorePlatform/Concurrency/bcFuture.h"
#include "CorePlatformImp/CorePlatformImpPCH.h"

namespace black_cat
{
	namespace core_platform
	{
		// TODO Write bcFutureProvider, bcSharedFutureProvider, bcPromiseProvider T& and void specialization /

		template<typename T>
		struct bc_future_pack<bc_platform::win32, T>
		{
			std::future<T> m_future;

			bc_future_pack()
			{
			}

			explicit bc_future_pack(std::future<T>&& p_future)
				: m_future(std::move(p_future))
			{
			}
		};

		template<typename T>
		struct bc_shared_future_pack<bc_platform::win32, T>
		{
			std::shared_future<T> m_future;

			bc_shared_future_pack()
			{
			}

			explicit bc_shared_future_pack(std::shared_future<T> p_future)
				: m_future(p_future)
			{
			}

			explicit bc_shared_future_pack(std::shared_future<T>&& p_future)
				: m_future(p_future)
			{
			}

			explicit bc_shared_future_pack(std::future<T>&& p_future)
				: m_future(p_future)
			{
			}
		};

		template<typename T>
		struct bc_promise_pack<bc_platform::win32, T>
		{
			std::promise<T> m_promise;

			explicit bc_promise_pack(std::promise<T>&& p_promise)
				: m_promise(std::move(p_promise))
			{
			}
		};

		template< typename T >
		class bc_shared_future_proxy< bc_platform::win32, T>;

		template< typename T >
		class bc_promise_proxy< bc_platform::win32, T >;

		template< bc_platform TPlatform, typename T >
		bc_future_proxy<TPlatform, T>::bc_future_proxy() noexcept(true)
			: m_pack()
		{
		};

		template< bc_platform TPlatform, typename T >
		bc_future_proxy<TPlatform, T>::bc_future_proxy(this_type&& p_other) noexcept(true)
			: m_pack(std::move(p_other.m_pack.m_future))
		{
		};

		template< bc_platform TPlatform, typename T >
		typename bc_future_proxy<TPlatform, T>::this_type& bc_future_proxy<TPlatform, T>::operator=(this_type&& p_other) noexcept(true)
		{
			m_pack.m_future = std::move(p_other.m_pack.m_future);

			return *this;
		};

		template< bc_platform TPlatform, typename T >
		typename bc_future_proxy<TPlatform, T>::shared_type bc_future_proxy<TPlatform, T>::share()
		{
			shared_type l_temp(std::move(*this));

			return l_temp;
		};

		template< bc_platform TPlatform, typename T >
		bool bc_future_proxy<TPlatform, T>::valid() const noexcept(true)
		{
			return m_pack.m_future.valid();
		};

		template< bc_platform TPlatform, typename T >
		typename bc_future_proxy<TPlatform, T>::type bc_future_proxy<TPlatform, T>::get()
		{
			return m_pack.m_future.get();
		};

		template< bc_platform TPlatform, typename T >
		void bc_future_proxy<TPlatform, T>::wait() const noexcept(true)
		{
			m_pack.m_future.wait();
		};

		template< bc_platform TPlatform, typename T >
		bc_future_status bc_future_proxy<TPlatform, T>::wait_for(const bcUINT64 p_nano) const
		{
			return m_pack.m_future.wait_for(std::chrono::nanoseconds(p_nano));
		};

		template< bc_platform TPlatform, typename T >
		bc_shared_future_proxy<TPlatform, T>::bc_shared_future_proxy() noexcept(true)
			: m_pack()
		{
		};

		template< bc_platform TPlatform, typename T >
		bc_shared_future_proxy<TPlatform, T>::bc_shared_future_proxy(const this_type& p_other) noexcept(true)
			: m_pack(p_other.m_pack.m_future)
		{
		};

		template< bc_platform TPlatform, typename T >
		bc_shared_future_proxy<TPlatform, T>::bc_shared_future_proxy(this_type&& p_other) noexcept(true)
			: m_pack(std::move(p_other.m_pack.m_future))
		{
		};

		template< bc_platform TPlatform, typename T >
		bc_shared_future_proxy<TPlatform, T>::bc_shared_future_proxy(future_type&& p_future)
			: m_pack(std::move(p_future))
		{
		}

		template< bc_platform TPlatform, typename T >
		bc_shared_future_proxy<TPlatform, T>::~bc_shared_future_proxy() noexcept(true)
		{
		}

		template< bc_platform TPlatform, typename T >
		typename bc_shared_future_proxy<TPlatform, T>::this_type& bc_shared_future_proxy<TPlatform, T>::operator=(const this_type& p_other)
		{
			m_pack.m_future = p_other.m_pack.m_future;

			return *this;
		}

		template< bc_platform TPlatform, typename T >
		typename bc_shared_future_proxy<TPlatform, T>::this_type& bc_shared_future_proxy<TPlatform, T>::operator=(this_type&& p_other)
		{
			m_pack.m_future = std::move(p_other.m_pack.m_future);

			return *this;
		};

		template< bc_platform TPlatform, typename T >
		bool bc_shared_future_proxy<TPlatform, T>::valid() const noexcept(true)
		{
			return m_pack.m_future.valid();
		};

		template< bc_platform TPlatform, typename T >
		typename bc_shared_future_proxy<TPlatform, T>::type bc_shared_future_proxy<TPlatform, T>::get() const
		{
			return m_pack.m_future.get();
		};

		template< bc_platform TPlatform, typename T >
		void bc_shared_future_proxy<TPlatform, T>::wait() const noexcept(true)
		{
			return m_pack.m_future.wait();
		};

		template< bc_platform TPlatform, typename T >
		bc_future_status bc_shared_future_proxy<TPlatform, T>::wait_for(const bcUINT64 p_nano) const
		{
			return static_cast<bc_future_status>(m_pack.m_future.wait_for(std::chrono::nanoseconds(p_nano)));
		}

		template< bc_platform TPlatform, typename T >
		bc_promise_proxy<TPlatform, T>::bc_promise_proxy() noexcept(true)
			: m_pack()
		{
		};
			
		// TODO: Check here
		/*template<typename Allocator>
		bcPromiseBase(std::allocator_arg_t, Allocator const&);*/

		template< bc_platform TPlatform, typename T >
		bc_promise_proxy<TPlatform, T>::bc_promise_proxy(this_type&& p_other) noexcept(true)
			: m_pack(std::move(p_other.m_pack.m_pack.m_promise))
		{
		};

		template< bc_platform TPlatform, typename T >
		bc_promise_proxy<TPlatform, T>::~bc_promise_proxy()
		{
		};

		template< bc_platform TPlatform, typename T >
		typename bc_promise_proxy<TPlatform, T>::this_type& bc_promise_proxy<TPlatform, T>::operator=(this_type&& p_other)
		{
			m_pack.m_pack.m_promise = std::move(p_other.m_pack.m_pack.m_promise);

			return *this;
		};

		template< bc_platform TPlatform, typename T >
		void bc_promise_proxy<TPlatform, T>::swap(this_type& p_other) noexcept(true)
		{
			m_pack.m_pack.m_promise.swap(p_other.m_pack.m_pack.m_promise);
		};

		template< bc_platform TPlatform, typename T >
		typename bc_promise_proxy<TPlatform, T>::future_type bc_promise_proxy<TPlatform, T>::get_future()
		{
			future_type lTemp;
			lTemp.m_future = std::move(m_pack.m_pack.m_promise.get_future());

			return lTemp;
		};

		template< bc_platform TPlatform, typename T >
		void bc_promise_proxy<TPlatform, T>::set_value(const type& p_value)
		{
			m_pack.m_promise.set_value(p_value);
		};

		template< bc_platform TPlatform, typename T >
		void bc_promise_proxy<TPlatform, T>::set_value(type&& p_value)
		{
			m_pack.m_promise.set_value(p_value);
		};

		template< bc_platform TPlatform, typename T >
		void bc_promise_proxy<TPlatform, T>::set_exception(std::exception_ptr p_exception)
		{
			m_pack.m_promise.set_exception(p_exception);
		};
	}
}