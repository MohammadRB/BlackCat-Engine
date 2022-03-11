
#include "Core/CorePCH.h"
#include "CorePlatformImp/Utility/bcHardwareInfo.h"
#include "Core/Concurrency/bcThreadManager.h"

namespace black_cat
{
	namespace core
	{
		bc_interrupt_flag::bc_interrupt_flag() : m_flag(false)
		{
		}

		bc_interrupt_flag::bc_interrupt_flag(bool pFlag) : m_flag(pFlag)
		{
		}

		bc_interrupt_flag::~bc_interrupt_flag() = default;
		
		bool bc_interrupt_flag::test_and_set()
		{
			return m_flag.test_and_set(platform::bc_memory_order::relaxed);
		}

		void bc_interrupt_flag::clear()
		{
			m_flag.clear(platform::bc_memory_order::relaxed);
		}

		bc_thread_manager::bc_thread_manager(bcSIZE p_hardware_thread_count, bcSIZE p_reserved_thread_count) noexcept
		{
			_initialize(p_hardware_thread_count, p_reserved_thread_count);
		}

		bc_thread_manager::~bc_thread_manager()
		{
			_stop_workers();
		}

		bcUINT32 bc_thread_manager::hardware_thread_count() const
		{
			return m_hardware_thread_count;
		}

		bcUINT32 bc_thread_manager::max_thread_count() const
		{
			return m_hardware_thread_count + m_reserved_thread_count;
		}

		bcUINT32 bc_thread_manager::spawned_thread_count() const
		{
			return m_spawned_thread_count.load(platform::bc_memory_order::relaxed);
		}

		bcUINT32 bc_thread_manager::task_count() const
		{
			return m_task_count.load(platform::bc_memory_order::seqcst);
		}

		void bc_thread_manager::interrupt_thread(platform::bc_thread::id p_thread_id)
		{
			{
				platform::bc_shared_lock<platform::bc_shared_mutex> l_guard(m_threads_mutex);

				for (bcUINT32 l_i = 0, l_c = m_threads.size(); l_i <l_c; ++l_i)
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

		void bc_thread_manager::_initialize(bcSIZE p_hardware_thread_count, bcSIZE p_reserved_thread_count)
		{
			m_hardware_thread_count = p_hardware_thread_count;
			m_reserved_thread_count = p_reserved_thread_count;
			m_done.store(false);
			m_spawned_thread_count.store(0, platform::bc_memory_order::relaxed);
			m_num_thread_in_spin.store(0, platform::bc_memory_order::relaxed);
			m_task_count.store(0, platform::bc_memory_order::relaxed);

			try
			{
				m_threads.reserve(m_hardware_thread_count + m_reserved_thread_count);

				for (bcUINT32 l_i = 0; l_i < m_hardware_thread_count; ++l_i)
				{
					_push_worker();
				}
			}
			catch (...)
			{
				_stop_workers();
				throw;
			}

			platform::bc_basic_hardware_info l_hardware_info{};
			platform::bc_hardware_info::get_basic_info(l_hardware_info);

			m_tasks_pool.initialize
			(
				m_hardware_thread_count * 2 + m_reserved_thread_count, 
				std::max(sizeof(bc_task_link<void>), l_hardware_info.m_cache_line_size),
				bc_alloc_type::program
			);
		}

		void bc_thread_manager::_restart_workers()
		{
			m_done.store(false);

			{
				platform::bc_lock_guard<platform::bc_shared_mutex> l_guard(m_threads_mutex);

				for(auto& l_thread : m_threads)
				{
					l_thread->m_thread = platform::bc_thread(&bc_thread_manager::_worker_spin, this, l_thread->m_my_index);
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
			const auto l_max_thread_count = max_thread_count();
			const auto l_spawned_thread_count = spawned_thread_count();
			if (l_spawned_thread_count >= l_max_thread_count)
			{
				return;
			}

			{
				platform::bc_shared_mutex_guard l_guard(m_threads_mutex);

				m_threads.push_back(bc_make_unique<_thread_data>
				(
					bc_alloc_type::program,
					l_spawned_thread_count,
					platform::bc_thread(&bc_thread_manager::_worker_spin, this, l_spawned_thread_count)
				));
				m_threads.back()->m_thread.set_name(bcL("BC_Worker"));
				m_threads.back()->m_thread.set_priority(platform::bc_thread_priority::highest);

				m_spawned_thread_count.fetch_add(1);
			}
		}

		void bc_thread_manager::_join_workers()
		{
			{
				platform::bc_shared_lock<platform::bc_shared_mutex> l_guard(m_threads_mutex);
				
				for (const auto& m_thread : m_threads)
				{
					if (m_thread->thread().joinable())
					{
						m_thread->thread().join();
					}
				}
			}
		}

		bool bc_thread_manager::_pop_task_from_global_queue(task_wrapper_type& p_task)
		{
			return m_global_queue.pop(p_task);
		}

		void bc_thread_manager::_worker_spin(bcUINT32 p_my_index)
		{
			{
				platform::bc_shared_lock<platform::bc_shared_mutex> l_guard(m_threads_mutex);
				// It is safe to get a pointer from vector because we have reserved needed memory in vector
				m_my_data.set(m_threads[p_my_index].get());
			}

			m_num_thread_in_spin.fetch_add(1);

			bcUINT32 l_without_task = 0;
			task_wrapper_type l_task;

			while (!m_done.load())
			{
				if (_pop_task_from_global_queue(l_task))
				{
					l_without_task = 0;
					m_task_count.fetch_sub(1);
					m_num_thread_in_spin.fetch_sub(1);

					// If number of steady threads in worker spin method is zero there is no need to notify another thread
					if constexpr (s_num_thread_in_spin != 0)
					{
						m_cvariable.notify_one();
					}

					l_task();
					l_task.reset();

					m_num_thread_in_spin.fetch_add(1);
				}
				else
				{
					++l_without_task;

					if (l_without_task <= s_worker_switch_threshold)
					{
						platform::bc_thread::current_thread_yield();
					}
					else
					{
						l_without_task = 0;

						if (m_num_thread_in_spin.load()> s_num_thread_in_spin)
						{
							{
								platform::bc_unique_lock<platform::bc_mutex> l_guard(m_cvariable_mutex);

								if (m_num_thread_in_spin.load(platform::bc_memory_order::relaxed)> s_num_thread_in_spin)
								{
									m_num_thread_in_spin.fetch_sub(1, platform::bc_memory_order::relaxed);

									m_cvariable.wait(l_guard);

									m_num_thread_in_spin.fetch_add(1, platform::bc_memory_order::relaxed);
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