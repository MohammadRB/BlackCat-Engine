
#include "Core/CorePCH.h"
#include "Core/Concurrency/bcThreadManager.h"

namespace black_cat
{
	namespace core
	{
		void bc_task_stealing_queue::push(delegate_type&& p_task)
		{
			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_lock(m_mutex);
				m_deque.push_front(std::move(p_task));
			}
		}

		bool bc_task_stealing_queue::empty() const
		{
			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_lock(m_mutex);
				return m_deque.empty();
			}
		}

		bool bc_task_stealing_queue::pop(delegate_type& p_task)
		{
			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_lock(m_mutex);
				if (m_deque.empty())
				{
					return false;
				}

				p_task = std::move(m_deque.front());
				m_deque.pop_front();

				return true;
			}
		}

		bool bc_task_stealing_queue::try_steal(delegate_type& p_task)
		{
			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_lock(m_mutex);
				if (m_deque.empty())
				{
					return false;
				}

				p_task = std::move(m_deque.back());
				m_deque.pop_back();

				return true;
			}
		}

		bc_interrupt_flag::bc_interrupt_flag() : m_flag(false)
		{
		}

		bc_interrupt_flag::bc_interrupt_flag(bool pFlag) : m_flag(pFlag)
		{
		}

		bc_interrupt_flag::~bc_interrupt_flag() = default;
		
		bool bc_interrupt_flag::test_and_set()
		{
			return m_flag.test_and_set(core_platform::bc_memory_order::relaxed);
		}

		void bc_interrupt_flag::clear()
		{
			m_flag.clear(core_platform::bc_memory_order::relaxed);
		}

		bc_thread_manager::bc_thread_manager(bcSIZE p_thread_count, bcSIZE p_reserved_thread_count) noexcept
			: m_my_data()
		{
			_initialize(p_thread_count, p_reserved_thread_count);
		}

		bc_thread_manager::~bc_thread_manager()
		{
			_stop_workers();
		}

		bcSIZE bc_thread_manager::max_thread_count() const
		{
			return m_thread_count + m_reserved_thread_count;
		}

		bcSIZE bc_thread_manager::spawned_thread_count() const
		{
			{
				core_platform::bc_shared_lock< core_platform::bc_shared_mutex > l_guard(m_threads_mutex);
				return m_threads.size();
			}
		}

		bcSIZE bc_thread_manager::task_count() const
		{
			return m_task_count.load(core_platform::bc_memory_order::seqcst);
		}

		void bc_thread_manager::interrupt_thread(core_platform::bc_thread::id p_thread_id)
		{
			{
				core_platform::bc_shared_lock< core_platform::bc_shared_mutex > l_guard(m_threads_mutex);

				for (bcUINT32 l_i = 0, l_c = m_threads.size(); l_i < l_c; ++l_i)
				{
					_thread_data* l_current = m_threads[l_i].get();
					if (l_current->thread().get_id() == p_thread_id)
					{
						l_current->interrupt_flag().test_and_set();
						return;
					}
				}

				BC_ASSERT(false);
			}
		}

		void bc_thread_manager::check_for_interruption()
		{
			_thread_data* l_my_data = m_my_data.get();
			bc_interrupt_flag& l_flag = l_my_data->interrupt_flag();

			if (l_flag.test_and_set())
			{
				l_flag.clear();
				throw bc_thread_interrupted_exception();
			}

			l_flag.clear();
		}

		void bc_thread_manager::_initialize(bcSIZE p_thread_count, bcSIZE p_reserved_thread_count)
		{
			m_thread_count = std::max(p_thread_count, 4U);
			m_reserved_thread_count = std::max(p_reserved_thread_count, m_thread_count);
			m_done.store(false);
			m_task_count.store(0, core_platform::bc_memory_order::relaxed);
			m_num_thread_in_spin.store(0, core_platform::bc_memory_order::relaxed);

			try
			{
				m_threads.reserve(m_thread_count + m_reserved_thread_count);

				for (bcUINT32 l_i = 0; l_i < m_thread_count; ++l_i)
				{
					_push_worker();
				}
			}
			catch (...)
			{
				_stop_workers();
				throw;
			}
		}

		void bc_thread_manager::_restart_workers()
		{
			m_done.store(false);

			{
				core_platform::bc_lock_guard< core_platform::bc_shared_mutex > l_guard(m_threads_mutex);

				for(auto& l_thread : m_threads)
				{
					l_thread->m_thread = core_platform::bc_thread(&bc_thread_manager::_worker_spin, this, l_thread->m_my_index);
				}
			}
		}

		void bc_thread_manager::_stop_workers()
		{
			m_done.store(true);
			m_cvariable.notify_all();

			_join_workers();
		}

		void bc_thread_manager::_push_worker()
		{
			{
				core_platform::bc_shared_mutex_guard l_guard(m_threads_mutex);

				auto l_my_index = m_threads.size();
				if (l_my_index >= m_thread_count + m_reserved_thread_count)
				{
					return;
				}

				m_threads.push_back(bc_make_unique< _thread_data >
				(
					bc_alloc_type::program,
					l_my_index,
					core_platform::bc_thread(&bc_thread_manager::_worker_spin, this, l_my_index)
				));
				m_threads.back()->m_thread.set_name(bcL("BC_Worker"));
				m_threads.back()->m_thread.set_priority(core_platform::bc_thread_priority::highest);
			}
		}

		void bc_thread_manager::_join_workers()
		{
			{
				core_platform::bc_shared_lock< core_platform::bc_shared_mutex > l_guard(m_threads_mutex);
				
				for (auto l_begin = m_threads.begin(), l_end = m_threads.end(); l_begin != l_end; ++l_begin)
				{
					if ((*l_begin)->thread().joinable())
					{
						(*l_begin)->thread().join();
					}
				}
			}
		}

		bool bc_thread_manager::_pop_task_from_local_queue(task_type& p_task)
		{
			_thread_data* l_my_data = m_my_data.get();

			return l_my_data && l_my_data->local_queue().pop(p_task);
		}

		bool bc_thread_manager::_pop_task_from_global_queue(task_type& p_task)
		{
			return m_global_queue.pop(p_task);
		}

		bool bc_thread_manager::_pop_task_from_others_queue(task_type& p_task)
		{
			_thread_data* l_my_data = m_my_data.get();

			{
				core_platform::bc_shared_lock< core_platform::bc_shared_mutex > l_guard(m_threads_mutex);

				for (bcUINT32 l_i = 0, l_c = m_threads.size(); l_i < l_c; ++l_i)
				{
					const bcUINT32 l_index = (l_my_data->index() + l_i + 1) % l_c;

					if (m_threads[l_index]->local_queue().try_steal(p_task))
					{
						return true;
					}
				}
			}

			return false;
		}

		void bc_thread_manager::_worker_spin(bcUINT32 p_my_index)
		{
			{
				core_platform::bc_shared_lock< core_platform::bc_shared_mutex > l_guard(m_threads_mutex);
				// It is safe to get a pointer from vector because we have reserved needed memory in vector
				m_my_data.set(m_threads[p_my_index].get());
			}

			m_num_thread_in_spin.fetch_add(1);

			bcUINT32 l_without_task = 0;
			task_type l_task;

			while (!m_done.load())
			{
				if 
				(
					_pop_task_from_local_queue(l_task) ||
					_pop_task_from_global_queue(l_task) ||
					_pop_task_from_others_queue(l_task)
				)
				{
					l_without_task = 0;
					m_task_count.fetch_sub(1);
					m_num_thread_in_spin.fetch_sub(1);

					if constexpr (s_num_thread_in_spin != 0)	// If number of steady threads in worker spin method is zero
					{											// there is no need to notify another thread
						m_cvariable.notify_one();
					}

					l_task(core_platform::bc_thread::current_thread_id());
					l_task.reset();

					m_num_thread_in_spin.fetch_add(1);
				}
				else
				{
					++l_without_task;

					if (l_without_task <= s_worker_switch_threshold)
					{
						core_platform::bc_thread::current_thread_yield();
					}
					else
					{
						l_without_task = 0;

						if (m_num_thread_in_spin.load() > s_num_thread_in_spin)
						{
							{
								core_platform::bc_unique_lock< core_platform::bc_mutex > l_guard(m_cvariable_mutex);

								if (m_num_thread_in_spin.load(core_platform::bc_memory_order::relaxed) > s_num_thread_in_spin)
								{
									m_num_thread_in_spin.fetch_sub(1, core_platform::bc_memory_order::relaxed);

									m_cvariable.wait(l_guard);

									m_num_thread_in_spin.fetch_add(1, core_platform::bc_memory_order::relaxed);
								}
							}
						}
					}
				}
			}

			m_num_thread_in_spin.fetch_sub(1);
		}
	}
}