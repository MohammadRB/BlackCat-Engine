// [11/10/2014 MRB]

#pragma once

#include <future>
#include "CorePlatform/Concurrency/bcFuture.h"

namespace black_cat
{
	namespace platform
	{
		// TODO Write bcFutureProvider, bcSharedFutureProvider, bcPromiseProvider T& and void specialization /

		template<typename T>
		struct bc_platform_future_pack<bc_platform::win32, T>
		{
			bc_platform_future_pack()
			{
			}

			explicit bc_platform_future_pack(std::future<T>&& p_future)
				: m_future(std::move(p_future))
			{
			}

			std::future<T> m_future;
		};

		template<typename T>
		struct bc_platform_shared_future_pack<bc_platform::win32, T>
		{
			bc_platform_shared_future_pack()
			{
			}

			explicit bc_platform_shared_future_pack(std::shared_future<T> p_future)
				: m_future(p_future)
			{
			}

			explicit bc_platform_shared_future_pack(std::shared_future<T>&& p_future)
				: m_future(p_future)
			{
			}

			explicit bc_platform_shared_future_pack(std::future<T>&& p_future)
				: m_future(p_future)
			{
			}

			std::shared_future<T> m_future;
		};

		template<typename T>
		struct bc_platform_promise_pack<bc_platform::win32, T>
		{
			bc_platform_promise_pack()
			{
			}

			explicit bc_platform_promise_pack(std::promise<T>&& p_promise)
			{
			}

			std::promise<T> m_promise;
		};

		template<bc_platform TPlatform, typename T>
		class bc_platform_shared_future;

		template<bc_platform TPlatform, typename T>
		class bc_platform_promise;

		// -- bc_platform_future --------------------------------------------------------------------------------

		template<bc_platform TPlatform, typename T>
		bc_platform_future<TPlatform, T>::bc_platform_future() noexcept(true)
			: m_pack()
		{
		};

		template<bc_platform TPlatform, typename T>
		bc_platform_future<TPlatform, T>::bc_platform_future(this_type&& p_other) noexcept(true)
			: m_pack(std::move(p_other.m_pack.m_future))
		{
		}

		template<bc_platform TPlatform, typename T>
		bc_platform_future<TPlatform, T>::~bc_platform_future() noexcept(true)
		{
		}

		template<bc_platform TPlatform, typename T>
		typename bc_platform_future<TPlatform, T>::this_type& bc_platform_future<TPlatform, T>::operator=(this_type&& p_other) noexcept(true)
		{
			m_pack.m_future = std::move(p_other.m_pack.m_future);

			return *this;
		}

		template<bc_platform TPlatform, typename T>
		typename bc_platform_future<TPlatform, T>::shared_type bc_platform_future<TPlatform, T>::share()
		{
			shared_type l_temp(std::move(*this));

			return l_temp;
		}

		template<bc_platform TPlatform, typename T>
		bool bc_platform_future<TPlatform, T>::valid() const noexcept(true)
		{
			return m_pack.m_future.valid();
		}

		template<bc_platform TPlatform, typename T>
		typename bc_platform_future<TPlatform, T>::type bc_platform_future<TPlatform, T>::get()
		{
			return m_pack.m_future.get();
		}

		template<bc_platform TPlatform, typename T>
		void bc_platform_future<TPlatform, T>::wait() const noexcept(true)
		{
			m_pack.m_future.wait();
		}

		template<bc_platform TPlatform, typename T>
		bc_future_status bc_platform_future<TPlatform, T>::wait_for(const std::chrono::nanoseconds& p_duration) const
		{
			return static_cast<bc_future_status>(m_pack.m_future.wait_for(p_duration));
		}

		template<bc_platform TPlatform, typename T>
		bc_future_status bc_platform_future<TPlatform, T>::wait_for(const std::chrono::microseconds& p_duration) const
		{
			return static_cast<bc_future_status>(m_pack.m_future.wait_for(p_duration));
		}

		template<bc_platform TPlatform, typename T>
		bc_future_status bc_platform_future<TPlatform, T>::wait_for(const std::chrono::milliseconds& p_duration) const
		{
			return static_cast<bc_future_status>(m_pack.m_future.wait_for(p_duration));
		}

		template<bc_platform TPlatform, typename T>
		bc_future_status bc_platform_future<TPlatform, T>::wait_for(const std::chrono::seconds& p_duration) const
		{
			return static_cast<bc_future_status>(m_pack.m_future.wait_for(p_duration));
		}

		// -- bc_platform_shared_future --------------------------------------------------------------------------------

		template<bc_platform TPlatform, typename T>
		bc_platform_shared_future<TPlatform, T>::bc_platform_shared_future() noexcept(true)
			: m_pack()
		{
		}

		template<bc_platform TPlatform, typename T>
		bc_platform_shared_future<TPlatform, T>::bc_platform_shared_future(const this_type& p_other) noexcept(true)
			: m_pack(p_other.m_pack.m_future)
		{
		}

		template<bc_platform TPlatform, typename T>
		bc_platform_shared_future<TPlatform, T>::bc_platform_shared_future(this_type&& p_other) noexcept(true)
			: m_pack(std::move(p_other.m_pack.m_future))
		{
		}

		template<bc_platform TPlatform, typename T>
		bc_platform_shared_future<TPlatform, T>::bc_platform_shared_future(future_type&& p_future)
			: m_pack(std::move(p_future))
		{
		}

		template<bc_platform TPlatform, typename T>
		bc_platform_shared_future<TPlatform, T>::~bc_platform_shared_future() noexcept(true)
		{
		}

		template<bc_platform TPlatform, typename T>
		typename bc_platform_shared_future<TPlatform, T>::this_type& bc_platform_shared_future<TPlatform, T>::operator=(const this_type& p_other)
		{
			m_pack.m_future = p_other.m_pack.m_future;

			return *this;
		}

		template<bc_platform TPlatform, typename T>
		typename bc_platform_shared_future<TPlatform, T>::this_type& bc_platform_shared_future<TPlatform, T>::operator=(this_type&& p_other) noexcept
		{
			m_pack.m_future = std::move(p_other.m_pack.m_future);

			return *this;
		}

		template<bc_platform TPlatform, typename T>
		bool bc_platform_shared_future<TPlatform, T>::valid() const noexcept(true)
		{
			return m_pack.m_future.valid();
		}

		template<bc_platform TPlatform, typename T>
		typename bc_platform_shared_future<TPlatform, T>::type bc_platform_shared_future<TPlatform, T>::get() const
		{
			return m_pack.m_future.get();
		}

		template<bc_platform TPlatform, typename T>
		void bc_platform_shared_future<TPlatform, T>::wait() const noexcept(true)
		{
			return m_pack.m_future.wait();
		}

		template<bc_platform TPlatform, typename T>
		bc_future_status bc_platform_shared_future<TPlatform, T>::wait_for(const std::chrono::nanoseconds& p_duration) const
		{
			return m_pack.m_future.wait_for(p_duration);
		}

		template<bc_platform TPlatform, typename T>
		bc_future_status bc_platform_shared_future<TPlatform, T>::wait_for(const std::chrono::microseconds& p_duration) const
		{
			return m_pack.m_future.wait_for(p_duration);
		}

		template<bc_platform TPlatform, typename T>
		bc_future_status bc_platform_shared_future<TPlatform, T>::wait_for(const std::chrono::milliseconds& p_duration) const
		{
			return m_pack.m_future.wait_for(p_duration);
		}

		template<bc_platform TPlatform, typename T>
		bc_future_status bc_platform_shared_future<TPlatform, T>::wait_for(const std::chrono::seconds& p_duration) const
		{
			return m_pack.m_future.wait_for(p_duration);
		}

		// -- bc_platform_promise --------------------------------------------------------------------------------

		template<bc_platform TPlatform, typename T>
		bc_platform_promise<TPlatform, T>::bc_platform_promise() noexcept(true)
			: m_pack()
		{
		}

		template<bc_platform TPlatform, typename T>
		bc_platform_promise<TPlatform, T>::bc_platform_promise(this_type&& p_other) noexcept(true)
			: m_pack(std::move(p_other.m_pack.m_promise))
		{
		}

		template<bc_platform TPlatform, typename T>
		bc_platform_promise<TPlatform, T>::~bc_platform_promise()
		{
		}

		template<bc_platform TPlatform, typename T>
		typename bc_platform_promise<TPlatform, T>::this_type& bc_platform_promise<TPlatform, T>::operator=(this_type&& p_other) noexcept
		{
			m_pack.m_promise = std::move(p_other.m_pack.m_promise);

			return *this;
		}

		template<bc_platform TPlatform, typename T>
		void bc_platform_promise<TPlatform, T>::swap(this_type& p_other) noexcept(true)
		{
			m_pack.m_promise.swap(p_other.m_pack.m_promise);
		}

		template<bc_platform TPlatform, typename T>
		typename bc_platform_promise<TPlatform, T>::future_type bc_platform_promise<TPlatform, T>::get_future()
		{
			future_type lTemp;
			lTemp.m_pack.m_future = std::move(m_pack.m_promise.get_future());

			return lTemp;
		}

		template<bc_platform TPlatform, typename T>
		void bc_platform_promise<TPlatform, T>::set_value(const type& p_value)
		{
			m_pack.m_promise.set_value(p_value);
		}

		template<bc_platform TPlatform, typename T>
		void bc_platform_promise<TPlatform, T>::set_value(type&& p_value)
		{
			m_pack.m_promise.set_value(p_value);
		}

		template<bc_platform TPlatform, typename T>
		void bc_platform_promise<TPlatform, T>::set_exception(std::exception_ptr p_exception)
		{
			m_pack.m_promise.set_exception(p_exception);
		}

		template<bc_platform TPlatform>
		bc_platform_promise<TPlatform, void>::bc_platform_promise() noexcept(true)
			: m_pack()
		{
		}

		template<bc_platform TPlatform>
		bc_platform_promise<TPlatform, void>::bc_platform_promise(this_type&& p_other) noexcept(true)
			: m_pack(std::move(p_other.m_pack.m_promise))
		{
		}

		template<bc_platform TPlatform>
		bc_platform_promise<TPlatform, void>::~bc_platform_promise()
		{
		}

		template<bc_platform TPlatform>
		typename bc_platform_promise<TPlatform, void>::this_type& bc_platform_promise<TPlatform, void>::operator=(this_type&& p_other) noexcept
		{
			m_pack.m_promise = std::move(p_other.m_pack.m_promise);

			return *this;
		}

		template<bc_platform TPlatform>
		void bc_platform_promise<TPlatform, void>::swap(this_type& p_other) noexcept(true)
		{
			m_pack.m_promise.swap(p_other.m_pack.m_promise);
		}

		template<bc_platform TPlatform>
		typename bc_platform_promise<TPlatform, void>::future_type bc_platform_promise<TPlatform, void>::get_future()
		{
			future_type lTemp;
			lTemp.m_pack.m_future = std::move(m_pack.m_promise.get_future());

			return lTemp;
		}

		template<bc_platform TPlatform>
		void bc_platform_promise<TPlatform, void>::set_value()
		{
			m_pack.m_promise.set_value();
		}

		template<bc_platform TPlatform>
		void bc_platform_promise<TPlatform, void>::set_exception(std::exception_ptr p_exception)
		{
			m_pack.m_promise.set_exception(p_exception);
		}
	}
}