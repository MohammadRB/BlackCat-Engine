//  [24/1/2015 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcFuture.h"
#include "CorePlatformImp/Concurrency/bcThread.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcDelegate.hpp"
#include "Core/bcEvent.h"
#include "Core/bcException.h"

namespace black_cat
{
	namespace core
	{
		template<typename T>
		class bc_task_link;

		class bc_thread_manager;

		template<typename T>
		class bc_task : platform::bc_no_copy
		{
			using value_type = T;
			using this_type = bc_task<T>;
			friend class bc_task_link<T>;

		public:
			bc_task() = default;

			bc_task(this_type&& p_other) noexcept(std::is_nothrow_move_constructible_v<platform::bc_future<value_type>>)
				: m_future(std::move(p_other.m_future)),
				m_thread_id_future(std::move(p_other.m_thread_id_future))
			{
			}

			~bc_task() = default;

			this_type& operator =(this_type&& p_other) noexcept(std::is_nothrow_move_assignable_v<platform::bc_future<value_type>>)
			{
				m_future = std::move(p_other.m_future);
				m_thread_id_future = std::move(p_other.m_thread_id_future);

				return *this;
			}

			bool is_ready() const noexcept(noexcept(std::declval<platform::bc_future<value_type>>().wait_for(std::chrono::seconds(0))))
			{
				return wait_for(std::chrono::seconds(0)) == platform::bc_future_status::ready;
			}

			bool valid() const noexcept(noexcept(std::declval<platform::bc_future<value_type>>().valid()))
			{
				return m_future.valid();
			}
			
			value_type get() noexcept(noexcept(std::declval<platform::bc_future<value_type>>().get()))
			{
				return m_future.get();
			}
			
			/**
			 * \brief Waits for the result to become available or return immediately if the task does not have shared state 
			 */
			void wait() const noexcept(noexcept(std::declval<platform::bc_future<value_type>>().wait()))
			{
				if(!m_future.valid())
				{
					return;
				}

				m_future.wait();
			}
			
			platform::bc_future_status wait_for(const std::chrono::nanoseconds& p_duration) const noexcept(noexcept(std::declval<platform::bc_future<value_type>>().wait_for(p_duration)))
			{
				if (!m_future.valid())
				{
					return platform::bc_future_status::ready;
				}
				
				return m_future.wait_for(p_duration);
			}

			platform::bc_future_status wait_for(const std::chrono::microseconds& p_duration) const noexcept(noexcept(std::declval<platform::bc_future<value_type>>().wait_for(p_duration)))
			{
				if (!m_future.valid())
				{
					return platform::bc_future_status::ready;
				}
				
				return m_future.wait_for(p_duration);
			}

			platform::bc_future_status wait_for(const std::chrono::milliseconds& p_duration) const noexcept(noexcept(std::declval<platform::bc_future<value_type>>().wait_for(p_duration)))
			{
				if (!m_future.valid())
				{
					return platform::bc_future_status::ready;
				}
				
				return m_future.wait_for(p_duration);
			}

			platform::bc_future_status wait_for(const std::chrono::seconds& p_duration) const noexcept(noexcept(std::declval<platform::bc_future<value_type>>().wait_for(p_duration)))
			{
				if (!m_future.valid())
				{
					return platform::bc_future_status::ready;
				}
				
				return m_future.wait_for(p_duration);
			}

			/**
			 * \brief When executor thread see this interrupt, it will throw a bc_thread_interrupted_exception
			 */
			void interrupt_executor_thread()
			{
				platform::bc_thread::id l_executor_thread_id = m_thread_id_future.get();
				bc_get_service<bc_thread_manager>()->interrupt_thread(l_executor_thread_id);
			}

		private:
			platform::bc_future<value_type> m_future;
			platform::bc_future<platform::bc_thread::id> m_thread_id_future;
		};

		template<typename T>
		class bc_task_link : platform::bc_no_copy
		{
		private:
			using value_type = T;
			using task_delegate_type = bc_delegate<value_type()>;
			using this_type = bc_task_link<value_type>;

		public:
			explicit bc_task_link(task_delegate_type p_del) noexcept
				: m_del(std::move(p_del))
			{
			}

			bc_task_link(this_type&& p_other) noexcept(std::is_nothrow_move_constructible_v<platform::bc_promise<value_type>>)
			{
				m_del = std::move(p_other.m_del);
				m_promise = std::move(p_other.m_promise);
				m_thread_id_promise = std::move(p_other.m_thread_id_promise);
			}

			~bc_task_link() = default;

			this_type& operator=(this_type&& p_other) noexcept(std::is_nothrow_move_assignable_v<platform::bc_promise<value_type>>)
			{
				m_del = std::move(p_other.m_del);
				m_promise = std::move(p_other.m_promise);
				m_thread_id_promise = std::move(p_other.m_thread_id_promise);

				return *this;
			}

			void operator()()
			{
				const auto l_thread_id = platform::bc_thread::current_thread_id();

				try
				{
					_call(l_thread_id, typename std::is_same<std::remove_reference_t<value_type>, void>::type());
				}
				catch (const std::exception& p_exp)
				{
					auto l_msg = bc_string("Task with thread id " + bc_to_string(l_thread_id) + " exited with error: ") + p_exp.what();
					m_promise.set_exception(std::make_exception_ptr(bc_thread_resource_exception(l_msg.c_str())));
					bc_app_event_error l_error_event(std::move(l_msg), true);
					bc_get_service<bc_event_manager>()->process_event(l_error_event);
				}
				catch (...)
				{
					auto l_msg = bc_string("Task with thread id " + bc_to_string(l_thread_id) + " exited with unknown error.");
					m_promise.set_exception(std::make_exception_ptr(bc_thread_resource_exception(l_msg.c_str())));
					bc_app_event_error l_error_event(std::move(l_msg), true);
					bc_get_service<bc_event_manager>()->process_event(l_error_event);
				}
			}

			bc_task<value_type> get_task()
			{
				bc_task<value_type> l_task;

				l_task.m_future = m_promise.get_future();
				l_task.m_thread_id_future = m_thread_id_promise.get_future();

				return l_task;
			}

		private:
			void _call(platform::bc_thread::id p_thread_id, std::true_type)
			{
				// Set executor thread id so in bcTask we can access to this id
				m_thread_id_promise.set_value(p_thread_id);
				m_del();
				m_promise.set_value();
			}

			void _call(platform::bc_thread::id p_thread_id, std::false_type)
			{
				// Set executor thread id so in bcTask we can access to this id
				m_thread_id_promise.set_value(p_thread_id);
				value_type lResult = m_del();
				m_promise.set_value(lResult);
			}

			task_delegate_type m_del;
			platform::bc_promise<value_type> m_promise;
			platform::bc_promise<platform::bc_thread::id> m_thread_id_promise;
		};
	}
}