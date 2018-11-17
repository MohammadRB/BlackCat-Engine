//  [1/24/2015 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcFuture.h"
#include "CorePlatformImp/Concurrency/bcThread.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcDelegate.hpp"

namespace black_cat
{
	namespace core
	{
		template< typename T >
		class bc_task_link;

		class bc_thread_manager;

		template<typename T>
		class bc_task : private core_platform::bc_no_copy
		{
			using type = T;
			using this_type = bc_task<T>;
			friend class bc_task_link<T>;

		public:
			bc_task() = default;

			bc_task(this_type&& p_other) noexcept(std::is_nothrow_move_constructible<core_platform::bc_future<type>>::value)
				: m_future(std::move(p_other.m_future)),
				m_thread_id_future(std::move(p_other.m_thread_id_future))
			{
			}

			~bc_task() = default;

			this_type& operator =(this_type&& p_other) noexcept(std::is_nothrow_move_assignable<core_platform::bc_future<type>>::value)
			{
				m_future = std::move(p_other.m_future);
				m_thread_id_future = std::move(p_other.m_thread_id_future);

				return *this;
			}

			type get() noexcept(noexcept(std::declval<core_platform::bc_future<type>>().get()))
			{
				return m_future.get();
			}

			void wait() const noexcept(noexcept(std::declval<core_platform::bc_future<type>>().wait()))
			{
				m_future.wait();
			}

			core_platform::bc_future_status wait_for(const bcUINT64 p_nano) const noexcept(core_platform::bc_future<type>::wait_for)
			{
				return m_future.wait_for(std::chrono::nanoseconds(p_nano));
			}

			/**
			 * \brief When executor thread see this interrupt, it will throw a bc_thread_interrupted_exception
			 */
			void interrupt_executor_thread()
			{
				core_platform::bc_thread::id l_executor_thread_id = m_thread_id_future.get();
				bc_get_service< bc_thread_manager >()->interrupt_thread(l_executor_thread_id);
			}

		protected:

		private:
			core_platform::bc_future<type> m_future;
			core_platform::bc_future<core_platform::bc_thread::id> m_thread_id_future;
		};

		template<typename T>
		class bc_task_link : private core_platform::bc_no_copy
		{
		private:
			using type = T;
			using task_delegate_type = bc_delegate< type() >;
			using this_type = bc_task_link<type>;

		public:
			explicit bc_task_link(task_delegate_type&& p_del)
				: m_del(std::move(p_del))
			{
			}

			bc_task_link(this_type&& p_other) noexcept(std::is_nothrow_move_constructible<core_platform::bc_promise< type >>::value)
			{
				m_del = std::move(p_other.m_del);
				m_promise = std::move(p_other.m_promise);
				m_thread_id_promise = std::move(p_other.m_thread_id_promise);
			}

			~bc_task_link() = default;

			this_type& operator=(this_type&& p_other) noexcept(std::is_nothrow_move_assignable<core_platform::bc_promise< type >>::value)
			{
				m_del = std::move(p_other.m_del);
				m_promise = std::move(p_other.m_promise);
				m_thread_id_promise = std::move(p_other.m_thread_id_promise);

				return *this;
			}

			void operator ()(core_platform::bc_thread::id p_thread_id)
			{
				_call(p_thread_id, std::is_same< typename std::remove_reference< type >::type, void >::type());
			}

			bc_task<type> get_task()
			{
				bc_task<type> l_task;

				l_task.m_future = m_promise.get_future();
				l_task.m_thread_id_future = m_thread_id_promise.get_future();

				return l_task;
			}

		protected:

		private:
			void _call(core_platform::bc_thread::id p_thread_id, std::true_type)
			{
				try
				{
					// Set executor thread id so in bcTask we can access to this id
					m_thread_id_promise.set_value(p_thread_id);
					m_del();
					m_promise.set_value();
				}
				catch (...)
				{
					m_promise.set_exception(std::current_exception());
				}
			}

			void _call(core_platform::bc_thread::id p_thread_id, std::false_type)
			{
				try
				{
					// Set executor thread id so in bcTask we can access to this id
					m_thread_id_promise.set_value(p_thread_id);
					type lResult = m_del();
					m_promise.set_value(lResult);
				}
				catch (...)
				{
					m_promise.set_exception(std::current_exception());
				}
			}

			task_delegate_type m_del;
			core_platform::bc_promise< type > m_promise;
			core_platform::bc_promise< core_platform::bc_thread::id > m_thread_id_promise;
		};
	}
}