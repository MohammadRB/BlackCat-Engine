//  [1/24/2015 MRB]

#pragma once

#include "CorePlatform/Concurrency/bcConcurrencyDef.h"
#include "CorePlatformImp/Concurrency/bcFuture.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Concurrency/bcThread.h"
#include "CorePlatformImp/Concurrency/bcThreadLocal.h"
#include "Core/CorePCH.h"
#include "Core/bcConstant.h"
#include "Core/bcException.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcConcurrentQueue.h"
#include "Core/Container/bcDeque.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcDelegate.hpp"

namespace black_cat
{
	namespace core
	{
		class bc_task_stealing_queue
		{
		public:
			using this_type = bc_task_stealing_queue;
			using task_type = bc_delegate<void(core_platform::bc_thread::id)>;

		public:
			bc_task_stealing_queue() = default;

			bc_task_stealing_queue(const this_type&) = delete;

			bc_task_stealing_queue(this_type&&) = delete;

			~bc_task_stealing_queue() = default;

			this_type& operator =(const this_type&) = delete;

			this_type& operator =(this_type&&) = delete;

			void push(task_type&& p_task)
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> l_lock(m_mutex);
				
				m_deque.push_front(std::move(p_task));
			}

			bool empty() const
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> l_lock(m_mutex);

				return m_deque.empty();
			}

			bool pop(task_type& p_task)
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> l_lock(m_mutex);

				if (m_deque.empty())
				{
					return false;
				}

				p_task = std::move(m_deque.front());
				m_deque.pop_front();

				return true;
			}

			bool try_steal(task_type& p_task)
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> l_lock(m_mutex);

				if (m_deque.empty())
				{
					return false;
				}

				p_task = std::move(m_deque.back());
				m_deque.pop_back();

				return true;
			}

		protected:

		private:
			//bc_deque_frame< task_type > m_deque; TODO
			bc_deque< task_type > m_deque;
			mutable core_platform::bc_mutex m_mutex;
		};

		struct bc_interrupt_flag
		{
		public:
			bc_interrupt_flag() 
				: m_flag(false)
			{
			}

			bc_interrupt_flag(bool pFlag)
				: m_flag(pFlag)
			{
			}

			bc_interrupt_flag(const bc_interrupt_flag& p_other)
			{
				m_flag.store(p_other.m_flag.load(core_platform::bc_memory_order::seqcst),
					core_platform::bc_memory_order::seqcst);
			}

			~bc_interrupt_flag() = default;

			bc_interrupt_flag& operator =(const bc_interrupt_flag& p_other)
			{
				m_flag.store(p_other.m_flag.load(core_platform::bc_memory_order::seqcst),
					core_platform::bc_memory_order::seqcst);

				return *this;
			}

			void set()
			{
				m_flag.store(true, core_platform::bc_memory_order::seqcst);
			}

			bool test_clear()
			{
				return m_flag.exchange(false);
			}

		protected:

		private:
			core_platform::bc_atomic<bool> m_flag;
		};

		enum class bc_task_creation_option
		{
			none,		// Push task in local task queue of current thread
			fairness	// Push task in global task queue 
		};

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
			
			// When executer thread see this interrupt, it will throw an exception
			void interrupt_executer_thread()
			{
				core_platform::bc_thread::id l_excecuter_thread_id = m_thread_id_future.get();
				bc_service_manager::get().get_service< bc_thread_manager >()->interrupt_thread(l_excecuter_thread_id);
			}

			/*static this_type start_new(bc_delegate< type(void) >&& p_delegate, 
				bc_task_creation_option p_option = bc_task_creation_option::none)
			{
				return bc_service_manager::get().get_service< bc_thread_manager >()->start_new_task(std::move(p_delegate), p_option);
			}*/

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

			bc_task_link(this_type&& p_other)
			{
				m_del = std::move(p_other.m_del);
				m_promise = std::move(p_other.m_promise);
				m_thread_id_promise = std::move(p_other.m_thread_id_promise);
			}

			~bc_task_link() = default;

			this_type& operator =(this_type&& p_other)
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

				l_task.m_future = std::move(_get_future());
				l_task.m_thread_id_future = std::move(_get_thread_id_future());

				return l_task;
			}

		protected:

		private:
			void _call(core_platform::bc_thread::id p_thread_id, std::true_type)
			{
				try
				{
					// Set executer thread id so in bcTask we can access to this id
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
					// Set executer thread id so in bcTask we can access to this id
					m_thread_id_promise.set_value(p_thread_id);
					type lResult = m_del();
					m_promise.set_value(lResult);
				}
				catch (...)
				{
					m_promise.set_exception(std::current_exception());
				}
			}

			core_platform::bc_future<type> _get_future()
			{
				return m_promise.get_future();
			}

			core_platform::bc_future<core_platform::bc_thread::id> _get_thread_id_future()
			{
				return m_thread_id_promise.get_future();
			}

			task_delegate_type m_del;
			core_platform::bc_promise< type > m_promise;
			core_platform::bc_promise< core_platform::bc_thread::id > m_thread_id_promise;
		};

		class bc_thread_manager : public bc_iservice
		{
		private:
			using task_type = bc_delegate<void(core_platform::bc_thread::id)>;

		public:
			bc_thread_manager(bcSIZE p_thread_count, bcSIZE p_additional_thread_count) noexcept(true)
				: m_my_data(&_thread_data_cleanup)
			{
				_initialize(p_thread_count, p_additional_thread_count);
			}

			bc_thread_manager(const bc_thread_manager&) = delete;

			bc_thread_manager(bc_thread_manager&& p_other)
			{
				operator=(std::move(p_other));
			}

			~bc_thread_manager()
			{
				_destroy();
			}

			bc_thread_manager& operator=(const bc_thread_manager&) = delete;

			bc_thread_manager& operator=(bc_thread_manager&& p_other)
			{
				m_thread_count = p_other.m_thread_count;
				m_additional_thread_count = p_other.m_additional_thread_count;
				m_task_count.store(p_other.m_task_count.load());
				m_done.store(p_other.m_done.load());
				m_threads = std::move(p_other.m_threads);
				m_global_queue = std::move(p_other.m_global_queue);
				m_my_data = std::move(p_other.m_my_data);

				return *this;
			}

			bcSIZE thread_count() const
			{
				core_platform::bc_shared_lock< core_platform::bc_shared_mutex > l_gaurd(m_shared_mutex);

				return m_threads.size();
			}

			bcSIZE task_count() const
			{
				return m_task_count.load(core_platform::bc_memory_order::seqcst);
			}

			void interrupt_thread(core_platform::bc_thread::id p_thread_id)
			{
				core_platform::bc_shared_lock< core_platform::bc_shared_mutex > l_gaurd(m_shared_mutex);

				for (bcUINT32 l_i = 0, l_c = m_threads.size(); l_i < l_c; ++l_i)
				{
					_thread_data* l_current = m_threads[l_i].get();
					if (l_current->thread().get_id() == p_thread_id)
					{
						l_current->interrupt_flag().set();
						return;
					}
				}

				bcAssert(false);
			}

			void check_for_interruption()
			{
				_thread_data* l_my_data = m_my_data.get();

				if (l_my_data->interrupt_flag().test_clear())
				{
					throw bc_thread_interrupted_exception();
				}

			}

			template<typename T>
			bc_task<T> start_new_task(bc_delegate< T(void) >&& p_delegate, bc_task_creation_option p_option = bc_task_creation_option::none)
			{
				bc_task_link<T> l_task_link(std::move(p_delegate));
				bc_task<T> l_task = l_task_link.get_task();

				task_type l_task_wrapper(task_type::make_from_big_object(bc_alloc_type::frame, std::move(l_task_link))); // TODO
				_thread_data* l_my_data = nullptr;

				if (p_option == bc_task_creation_option::none)
					l_my_data = m_my_data.get();

				if (l_my_data)
					l_my_data->local_queue().push(std::move(l_task_wrapper));
				else
					m_global_queue.push(std::move(l_task_wrapper));

				bcUINT32 l_task_count = m_task_count.fetch_add(1U, core_platform::bc_memory_order::seqcst);

				if (l_task_count >= s_new_thread_threshold)
					_push_worker();

				return l_task;
			}

			static const bcCHAR* service_name()
			{
				return g_srv_thread_manager;
			}

		protected:

		private:
			class _thread_data
			{
			public:
				_thread_data(bcUINT32 p_my_index, core_platform::bc_thread&& p_thread)
					: m_my_index(p_my_index),
					m_local_queue(),
					m_interrupt_flag(),
					m_thread(std::move(p_thread))
				{
				}

				_thread_data(const _thread_data&) = delete;

				_thread_data(_thread_data&&) = delete;

				~_thread_data() = default;

				_thread_data& operator=(const _thread_data&) = delete;

				_thread_data& operator=(_thread_data&&) = delete;

				bcUINT32 index() const
				{
					return m_my_index;
				}

				bc_task_stealing_queue& local_queue()
				{
					return m_local_queue;
				}

				bc_interrupt_flag& interrupt_flag()
				{
					return m_interrupt_flag;
				}

				core_platform::bc_thread& thread()
				{
					return m_thread;
				}

			protected:

			private:
				bcUINT32 m_my_index;
				bc_task_stealing_queue m_local_queue;
				bc_interrupt_flag m_interrupt_flag;
				core_platform::bc_thread m_thread;
			};

			void _initialize(bcSIZE p_thread_count, bcSIZE p_additional_thread_count)
			{
				m_thread_count = p_thread_count;
				m_additional_thread_count = p_additional_thread_count;
				m_task_count.store(0, core_platform::bc_memory_order::seqcst);
				m_done.store(false);

				try
				{
					m_threads.reserve(p_thread_count + p_additional_thread_count);

					for (bcUINT32 l_i = 0; l_i < p_thread_count; ++l_i)
					{
						_push_worker();
					}
				}
				catch (...)
				{
					_destroy();

					throw;
				}
			}

			void _destroy()
			{
				m_done.store(true);

				_join_workers();
			}

			static void _thread_data_cleanup(_thread_data*)
			{
			}

			void _push_worker()
			{
				core_platform::bc_lock_guard< core_platform::bc_shared_mutex > l_gaurd(m_shared_mutex);

				bcUINT32 l_my_index = m_threads.size();

				if (l_my_index >= m_thread_count + m_additional_thread_count)
					return;

				m_threads.push_back(bc_make_unique<_thread_data>(
					bc_alloc_type::program,
					l_my_index,
					core_platform::bc_thread(&bc_thread_manager::_worker_spin, this, l_my_index)));
			}

			// TODO create pop mechanism for additional threads

			void _join_workers()
			{
				core_platform::bc_shared_lock< core_platform::bc_shared_mutex > l_gaurd(m_shared_mutex);

				for (auto l_begin = m_threads.begin(), l_end = m_threads.end(); l_begin != l_end; ++l_begin)
				{
					if ((*l_begin)->thread().joinable())
						(*l_begin)->thread().join();
				}
			}

			bool _pop_task_from_local_queue(task_type& p_task)
			{
				_thread_data* l_my_data = m_my_data.get();

				return l_my_data && l_my_data->local_queue().pop(p_task);
			}

			bool _pop_task_from_global_queue(task_type& p_task)
			{
				return m_global_queue.pop(p_task);
			}

			bool _pop_task_from_others_queue(task_type& p_task)
			{
				_thread_data* l_my_data = m_my_data.get();

				core_platform::bc_shared_lock< core_platform::bc_shared_mutex > l_gaurd(m_shared_mutex);

				for (bcUINT32 l_i = 0, l_c = m_threads.size(); l_i < l_c; ++l_i)
				{
					const bcUINT32 l_index = (l_my_data->index() + l_i + 1) % l_c;

					if (m_threads[l_index]->local_queue().try_steal(p_task))
						return true;
				}

				return false;
			}

			void _worker_spin(bcUINT32 p_my_index)
			{
				{
					core_platform::bc_shared_lock< core_platform::bc_shared_mutex > l_gaurd(m_shared_mutex);

					// It is safe to get a pointer from vector because we have reserved needed memory in vector
					m_my_data.set(m_threads[p_my_index].get());
				}

				bcUINT32 l_without_task = 0;
				task_type l_task;

				while (!m_done.load())
				{
					if (_pop_task_from_local_queue(l_task) ||
						_pop_task_from_global_queue(l_task) ||
						_pop_task_from_others_queue(l_task))
					{
						l_without_task = 0;
						m_task_count.fetch_sub(1U, core_platform::bc_memory_order::seqcst);

						l_task(core_platform::bc_thread::current_thread_id());
						l_task.reset();
					}
					else
					{
						++l_without_task;

						if (l_without_task <= s_worker_switch_threshold)
							core_platform::bc_thread::current_thread_yield();
						else
							core_platform::bc_thread::current_thread_yield_switch();
					}
				}
			}

		private:
			static const bcUINT32 s_worker_switch_threshold = 10;
			static const bcUINT32 s_new_thread_threshold = 10;

			bcSIZE m_thread_count;
			bcSIZE m_additional_thread_count;
			core_platform::bc_atomic< bcSIZE > m_task_count;
			core_platform::bc_atomic< bool > m_done;

			bc_vector_a< bc_unique_ptr<_thread_data>, bc_allocator_program > m_threads;
			//bc_concurrent_queue_frame< task_wrapper_type > m_global_queue; TODO
			bc_concurrent_queue< task_type > m_global_queue;
			core_platform::bc_thread_local< _thread_data > m_my_data;
			mutable core_platform::bc_shared_mutex m_shared_mutex;
		};
	}
}