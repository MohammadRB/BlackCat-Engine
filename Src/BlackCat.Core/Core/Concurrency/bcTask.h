//  [1/24/2015 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcFuture.h"
#include "CorePlatformImp/Concurrency/bcThread.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcDelegate.hpp"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/bcEvent.h"
#include "Core/bcException.h"

namespace black_cat
{
	namespace core
	{
		template< typename T >
		class bc_task_link;

		class bc_thread_manager;

		template<typename T>
		class bc_task : core_platform::bc_no_copy
		{
			using value_type = T;
			using this_type = bc_task< T >;
			friend class bc_task_link< T >;

		public:
			bc_task() = default;

			bc_task(this_type&& p_other) noexcept(std::is_nothrow_move_constructible<core_platform::bc_future<value_type>>::value)
				: m_future(std::move(p_other.m_future)),
				m_thread_id_future(std::move(p_other.m_thread_id_future))
			{
			}

			~bc_task() = default;

			this_type& operator =(this_type&& p_other) noexcept(std::is_nothrow_move_assignable<core_platform::bc_future<value_type>>::value)
			{
				m_future = std::move(p_other.m_future);
				m_thread_id_future = std::move(p_other.m_thread_id_future);

				return *this;
			}

			value_type get() noexcept(noexcept(std::declval<core_platform::bc_future<value_type>>().get()))
			{
				return m_future.get();
			}

			bool is_ready() const noexcept(core_platform::bc_future<value_type>::wait_for)
			{
				return wait_for(std::chrono::seconds(0)) == core_platform::bc_future_status::ready;
			}
			
			void wait() const noexcept(noexcept(std::declval<core_platform::bc_future<value_type>>().wait()))
			{
				m_future.wait();
			}
			
			core_platform::bc_future_status wait_for(const std::chrono::nanoseconds& p_duration) const noexcept(core_platform::bc_future<value_type>::wait_for)
			{
				return m_future.wait_for(p_duration);
			}

			core_platform::bc_future_status wait_for(const std::chrono::microseconds& p_duration) const noexcept(core_platform::bc_future<value_type>::wait_for)
			{
				return m_future.wait_for(p_duration);
			}

			core_platform::bc_future_status wait_for(const std::chrono::milliseconds& p_duration) const noexcept(core_platform::bc_future<value_type>::wait_for)
			{
				return m_future.wait_for(p_duration);
			}

			core_platform::bc_future_status wait_for(const std::chrono::seconds& p_duration) const noexcept(core_platform::bc_future<value_type>::wait_for)
			{
				return m_future.wait_for(p_duration);
			}

			/**
			 * \brief When executor thread see this interrupt, it will throw a bc_thread_interrupted_exception
			 */
			void interrupt_executor_thread()
			{
				core_platform::bc_thread::id l_executor_thread_id = m_thread_id_future.get();
				bc_get_service< bc_thread_manager >()->interrupt_thread(l_executor_thread_id);
			}

		private:
			core_platform::bc_future<value_type> m_future;
			core_platform::bc_future<core_platform::bc_thread::id> m_thread_id_future;
		};

		template<typename T>
		class bc_task_link : core_platform::bc_no_copy
		{
		private:
			using value_type = T;
			using task_delegate_type = bc_delegate< value_type() >;
			using this_type = bc_task_link< value_type >;

		public:
			explicit bc_task_link(task_delegate_type&& p_del)
				: m_del(std::move(p_del))
			{
			}

			bc_task_link(this_type&& p_other) noexcept(std::is_nothrow_move_constructible<core_platform::bc_promise< value_type >>::value)
			{
				m_del = std::move(p_other.m_del);
				m_promise = std::move(p_other.m_promise);
				m_thread_id_promise = std::move(p_other.m_thread_id_promise);
			}

			~bc_task_link() = default;

			this_type& operator=(this_type&& p_other) noexcept(std::is_nothrow_move_assignable<core_platform::bc_promise< value_type >>::value)
			{
				m_del = std::move(p_other.m_del);
				m_promise = std::move(p_other.m_promise);
				m_thread_id_promise = std::move(p_other.m_thread_id_promise);

				return *this;
			}

			void operator ()(core_platform::bc_thread::id p_thread_id)
			{
				try
				{
					_call(p_thread_id, std::is_same< typename std::remove_reference< value_type >::type, void >::type());
				}
				catch (const std::exception& p_exp)
				{
					auto l_msg = bc_string("Task with thread id " + bc_to_string(p_thread_id) + " exited with error: ") + p_exp.what();

					m_promise.set_exception(std::make_exception_ptr(bc_thread_resource_exception(l_msg.c_str())));

					bc_app_event_debug l_debug_event(std::move(l_msg));
					bc_get_service< bc_event_manager >()->process_event(l_debug_event);
				}
				catch (...)
				{
					auto l_msg = bc_string("Task with thread id " + bc_to_string(p_thread_id) + " exited with unknown error.");
					
					m_promise.set_exception(std::make_exception_ptr(bc_thread_resource_exception(l_msg.c_str())));

					bc_app_event_debug l_debug_event(std::move(l_msg));
					bc_get_service< bc_event_manager >()->process_event(l_debug_event);
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
			void _call(core_platform::bc_thread::id p_thread_id, std::true_type)
			{
				// Set executor thread id so in bcTask we can access to this id
				m_thread_id_promise.set_value(p_thread_id);
				m_del();
				m_promise.set_value();
			}

			void _call(core_platform::bc_thread::id p_thread_id, std::false_type)
			{
				// Set executor thread id so in bcTask we can access to this id
				m_thread_id_promise.set_value(p_thread_id);
				value_type lResult = m_del();
				m_promise.set_value(lResult);
			}

			task_delegate_type m_del;
			core_platform::bc_promise< value_type > m_promise;
			core_platform::bc_promise< core_platform::bc_thread::id > m_thread_id_promise;
		};
	}
}