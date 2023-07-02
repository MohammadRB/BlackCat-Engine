
#include "Core/CorePCH.h"
#include "CorePlatformImp/Utility/bcHardwareInfo.h"
#include "Core/Concurrency/bcThreadManager.h"

namespace black_cat
{
	namespace core
	{
		constexpr bcUINT bc_thread_manager::s_thread_sleep_threshold = 20;
		constexpr bcUINT bc_thread_manager::s_push_thread_threshold = 5;
		constexpr bcUINT bc_thread_manager::s_pop_thread_threshold = 10000;
		constexpr bcUINT bc_thread_manager::s_thread_in_spin_count = 0;
		
		bc_thread_manager::bc_thread_manager(bcUINT p_hardware_thread_count, bcUINT p_reserved_thread_count) noexcept
			: m_hardware_thread_count(0),
			m_reserved_thread_count(0)
		{
			_initialize(p_hardware_thread_count, p_reserved_thread_count);
		}

		bc_thread_manager::~bc_thread_manager()
		{
			_stop_workers();
		}

		bcUINT bc_thread_manager::hardware_thread_count() const
		{
			return m_hardware_thread_count;
		}

		bcUINT bc_thread_manager::max_thread_count() const
		{
			return m_hardware_thread_count + m_reserved_thread_count;
		}

		bcUINT bc_thread_manager::spawned_thread_count() const
		{
			return m_spawned_thread_count.load(platform::bc_memory_order::relaxed);
		}

		bcUINT bc_thread_manager::task_count() const
		{
			return m_task_count.load(platform::bc_memory_order::seqcst);
		}

		void bc_thread_manager::interrupt_thread(platform::bc_thread::id p_thread_id)
		{
			{
				platform::bc_shared_lock l_guard(m_threads_mutex);

				for (auto& l_thread : m_threads)
				{
					auto* l_thread_data = l_thread.get();
					if (l_thread_data->m_thread.get_id() == p_thread_id)
					{
						l_thread_data->test_and_set_interrupt_flag();
						return;
					}
				}

				BC_ASSERT(false);
			}
		}

		void bc_thread_manager::check_for_interruption()
		{
			auto& l_my_data = *m_my_data.get();

			if (l_my_data.test_and_set_interrupt_flag())
			{
				l_my_data.clear_interrupt_flag();
				throw bc_thread_interrupted_exception();
			}

			l_my_data.clear_interrupt_flag();
		}

		void bc_thread_manager::_initialize(bcSIZE p_hardware_thread_count, bcSIZE p_reserved_thread_count)
		{
			m_hardware_thread_count = p_hardware_thread_count;
			m_reserved_thread_count = p_reserved_thread_count;
			m_done.store(false);
			m_spawned_thread_count.store(0, platform::bc_memory_order::relaxed);
			m_thread_in_spin_count.store(0, platform::bc_memory_order::relaxed);
			m_task_count.store(0, platform::bc_memory_order::relaxed);

			const auto l_pool_size = m_hardware_thread_count * 2 + m_reserved_thread_count;
			platform::bc_basic_hardware_info l_hardware_info{};
			platform::bc_hardware_info::get_basic_info(l_hardware_info);

			m_queue_pool.initialize
			(
				l_pool_size,
				std::max(sizeof(task_queue_type::node_type), l_hardware_info.m_cache_line_size),
				bc_alloc_type::program
			);
			m_task_pool.initialize
			(
				l_pool_size,
				std::max(sizeof(bc_task_link<void>), l_hardware_info.m_cache_line_size),
				bc_alloc_type::program
			);
			m_queue.reset(task_queue_type(bc_object_pool_allocator<task_wrapper_type>(m_queue_pool)));

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
		}

		void bc_thread_manager::_push_worker()
		{
			const auto l_max_thread_count = max_thread_count();
			auto l_spawned_thread_count = spawned_thread_count();
			if (l_spawned_thread_count >= l_max_thread_count)
			{
				return;
			}

			{
				platform::bc_shared_mutex_guard l_guard(m_threads_mutex);

				l_spawned_thread_count = spawned_thread_count();
				if (l_spawned_thread_count >= l_max_thread_count)
				{
					return;
				}

				_thread_data* l_spawned_thread;

				if(m_threads.size() == l_max_thread_count)
				{
					const auto l_free_slot_ite = std::find_if
					(
						std::begin(m_threads),
						std::end(m_threads),
						[](const auto& p_thread)
						{
							return p_thread->is_done_requested() && p_thread->is_done();
						}
					);

					if (l_free_slot_ite == std::end(m_threads))
					{
						return;
					}

					l_spawned_thread = l_free_slot_ite->get();

					if(l_spawned_thread->m_thread.joinable())
					{
						l_spawned_thread->m_thread.join();
					}

					l_spawned_thread->reset_done_flags();
					l_spawned_thread->m_thread = platform::bc_thread(&bc_thread_manager::_worker_spin, this, l_spawned_thread->m_my_index);
				}
				else
				{
					m_threads.push_back
					(
						bc_make_unique<_thread_data>
						(
							bc_alloc_type::program,
							l_spawned_thread_count,
							platform::bc_thread(&bc_thread_manager::_worker_spin, this, l_spawned_thread_count)
						)
					);
					l_spawned_thread = m_threads.back().get();
				}

				l_spawned_thread->m_thread.set_name(bcL("BC_Worker"));
				l_spawned_thread->m_thread.set_priority(platform::bc_thread_priority::highest);

				m_spawned_thread_count.fetch_add(1);
			}
		}

		void bc_thread_manager::_pop_worker()
		{
			const auto l_min_thread_count = hardware_thread_count();
			auto l_spawned_thread_count = spawned_thread_count();
			if (l_spawned_thread_count <= l_min_thread_count)
			{
				return;
			}

			{
				platform::bc_shared_mutex_guard l_guard(m_threads_mutex);

				l_spawned_thread_count = spawned_thread_count();
				if (l_spawned_thread_count <= l_min_thread_count)
				{
					return;
				}

				const auto l_alive_slot = std::find_if
				(
					std::begin(m_threads),
					std::end(m_threads),
					[](const auto& p_thread)
					{
						return !p_thread->is_done_requested();
					}
				);
				(*l_alive_slot)->set_done_request();
				m_spawned_thread_count.fetch_sub(1);
			}
		}

		void bc_thread_manager::_restart_workers()
		{
			m_done.store(false);

			{
				platform::bc_lock_guard l_guard(m_threads_mutex);

				for(const auto& l_thread : m_threads)
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
		
		void bc_thread_manager::_join_workers()
		{
			{
				platform::bc_shared_lock l_guard(m_threads_mutex);
				
				for (const auto& l_thread : m_threads)
				{
					if (l_thread->m_thread.joinable())
					{
						l_thread->m_thread.join();
					}
				}
			}
		}

		bool bc_thread_manager::_pop_task_from_global_queue(task_wrapper_type& p_task)
		{
			return m_queue->pop(p_task);
		}

		void bc_thread_manager::_worker_spin(bcUINT p_my_index)
		{
			{
				platform::bc_shared_lock l_guard(m_threads_mutex);
				// It is safe to get a pointer from vector because we have reserved needed memory in vector
				m_my_data.set(m_threads[p_my_index].get());
			}

			m_thread_in_spin_count.fetch_add(1, platform::bc_memory_order::relaxed);

			bcUINT32 l_without_task = 0;
			task_wrapper_type l_task;

			while (!m_done.load(platform::bc_memory_order::relaxed) && !m_my_data->is_done_requested())
			{
				if (_pop_task_from_global_queue(l_task))
				{
					l_without_task = 0;
					m_task_count.fetch_sub(1, platform::bc_memory_order::relaxed);
					m_thread_in_spin_count.fetch_sub(1, platform::bc_memory_order::relaxed);

					// If number of steady threads in worker spin method is zero there is no need to notify another thread
					if constexpr (s_thread_in_spin_count != 0)
					{
						m_cvariable.notify_one();
					}

					l_task();
					l_task.reset();

					m_thread_in_spin_count.fetch_add(1, platform::bc_memory_order::relaxed);
				}
				else
				{
					++l_without_task;

					if (l_without_task <= s_thread_sleep_threshold)
					{
						platform::bc_thread::current_thread_yield();
					}
					else
					{
						l_without_task = 0;

						if (m_thread_in_spin_count.load(platform::bc_memory_order::relaxed) > s_thread_in_spin_count)
						{
							{
								platform::bc_unique_lock l_guard(m_cvariable_mutex);

								if (m_thread_in_spin_count.load(platform::bc_memory_order::relaxed) > s_thread_in_spin_count)
								{
									m_thread_in_spin_count.fetch_sub(1, platform::bc_memory_order::relaxed);

									m_cvariable.wait(l_guard);

									m_thread_in_spin_count.fetch_add(1, platform::bc_memory_order::relaxed);
								}
							}

							// If this thread is going to exit, another thread should be woke up to steal a task
							if (m_my_data->is_done_requested())
							{
								m_cvariable.notify_one();
							}
						}
					}
				}
			}

			m_thread_in_spin_count.fetch_sub(1);
			m_my_data->set_done();
		}
	}
}