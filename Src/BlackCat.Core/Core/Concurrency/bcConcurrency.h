// [05/14/2016 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Concurrency/bcThreadManager.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Utility/bcDelegate.h"
#include "Core/Utility/bcTemplateMetaType.h"
#include "Core/bcEvent.h"

namespace black_cat
{
	namespace core
	{
		class bc_concurrency
		{
		public:
			static core_platform::bc_thread::id current_thread_id();

			static void check_for_interruption();

			static bcUINT32 hardware_worker_count();

			static bcUINT32 spawned_worker_count();

			template<typename T>
			static bc_task<T> start_task(bc_delegate<T()> p_delegate, bc_task_creation_option p_option = bc_task_creation_option::policy_none);

			template<typename ...T>
			static void when_all(bc_task<T>&... p_tasks);

			template<typename T>
			static void when_all(std::initializer_list<bc_task<T>&> p_tasks);

			template<typename TIte, typename = std::enable_if_t<bc_is_iterator<TIte>::value>>
			static void when_all(TIte p_begin, TIte p_end);

			/**
			 * \brief 
			 * \tparam TIte								Iterator type
			 * \tparam TInitFunc	<TLocal(void)>			Function that initialize local data
			 * \tparam TBodyFunc	<TLocal(TLocal, Ite)>	Function that execute as for loop body
			 * \tparam TFinalFunc	<void(TLocal)>		Function that will be called by all started threads to accumulate results
			 * \param p_begin 
			 * \param p_end 
			 * \param p_init_func 
			 * \param p_body_func 
			 * \param p_finalizer_func 
			 */
			template<typename TIte, typename TInitFunc, typename TBodyFunc, typename TFinalFunc>
			static void concurrent_for_each(TIte p_begin, TIte p_end, TInitFunc p_init_func, TBodyFunc p_body_func, TFinalFunc p_finalizer_func);
			
			template<typename TIte, typename TInitFunc, typename TBodyFunc, typename TFinalFunc>
			static void concurrent_for_each(bcUINT32 p_thread_count, TIte p_begin, TIte p_end, TInitFunc p_init_func, TBodyFunc p_body_func, TFinalFunc p_finalizer_func);

			template<typename TIte, typename TBodyFunc>
			static void concurrent_for_each(TIte p_begin, TIte p_end, TBodyFunc p_body_func);

			template<typename TIte, typename TBodyFunc>
			static void concurrent_for_each(bcUINT32 p_thread_count, TIte p_begin, TIte p_end, TBodyFunc p_body_func);
			
			template<typename T>
			static T* double_check_lock(core_platform::bc_atomic<T*>& p_pointer, core_platform::bc_mutex& p_mutex, const bc_delegate<T*()>& p_initializer);

		private:
			template<typename TIte, typename TInitFunc, typename TBodyFunc, typename TFinalFunc>
			static void _concurrent_for_each(bcUINT32 p_thread_count, TIte p_begin, TIte p_end, TInitFunc p_init_func, TBodyFunc p_body_func, TFinalFunc p_finalizer_func);
			
			static bc_thread_manager* _get_thread_manager();
		};

		inline core_platform::bc_thread::id bc_concurrency::current_thread_id()
		{
			return core_platform::bc_thread::current_thread_id();
		}

		inline void bc_concurrency::check_for_interruption()
		{
			_get_thread_manager()->check_for_interruption();
		}

		inline bcUINT32 bc_concurrency::hardware_worker_count()
		{
			return _get_thread_manager()->hardware_thread_count();
		}

		inline bcUINT32 bc_concurrency::spawned_worker_count()
		{
			return _get_thread_manager()->spawned_thread_count();
		}

		template<typename T>
		bc_task<T> bc_concurrency::start_task(bc_delegate<T()> p_delegate, bc_task_creation_option p_option)
		{
			return _get_thread_manager()->start_new_task(std::move(p_delegate), p_option);
		}

		template<typename ...T>
		void bc_concurrency::when_all(bc_task<T>&... p_tasks)
		{
			[&](...) {}
			(
				(
					[&]()
					{
						p_tasks.wait();
						return true;
					}()
				)...
			);
		}

		template<typename T>
		void bc_concurrency::when_all(std::initializer_list<bc_task<T>&> p_tasks)
		{
			std::for_each(std::begin(p_tasks), std::end(p_tasks), [](const bc_task<T>& p_task)
			{
				p_task.wait();
			});
		}

		template<typename TIte, typename>
		void bc_concurrency::when_all(TIte p_begin, TIte p_end)
		{
			std::for_each(p_begin, p_end, [](const typename std::iterator_traits<TIte>::value_type& p_task)
			{
				p_task.wait();
			});
		}

		template<typename TIte, typename TInitFunc, typename TBodyFunc, typename TFinalFunc>
		void bc_concurrency::concurrent_for_each(TIte p_begin, TIte p_end, TInitFunc p_init_func, TBodyFunc p_body_func, TFinalFunc p_finalizer_func)
		{
			const bcUINT32 l_job_count = std::distance(p_begin, p_end);
			const bcUINT32 l_thread_count = std::min(hardware_worker_count(), l_job_count);

			concurrent_for_each
			(
				l_thread_count,
				p_begin,
				p_end,
				p_init_func,
				p_body_func,
				p_finalizer_func
			);
		}

		template<typename TIte, typename TInitFunc, typename TBodyFunc, typename TFinalFunc>
		void bc_concurrency::concurrent_for_each(bcUINT32 p_thread_count, TIte p_begin, TIte p_end, TInitFunc p_init_func, TBodyFunc p_body_func, TFinalFunc p_finalizer_func)
		{
			if(p_thread_count > 1)
			{
				_concurrent_for_each(p_thread_count, p_begin, p_end, p_init_func, p_body_func, p_finalizer_func);
			}
			else
			{
				auto l_init = p_init_func();
				std::for_each(p_begin, p_end, [&l_init, &p_body_func](typename std::iterator_traits<TIte>::reference p_item)
				{
					p_body_func(l_init, p_item);
				});
				p_finalizer_func(l_init);
			}
		}

		template<typename TIte, typename TBodyFunc>
		void bc_concurrency::concurrent_for_each(TIte p_begin, TIte p_end, TBodyFunc p_body_func)
		{
			const bcUINT32 l_job_count = std::distance(p_begin, p_end);
			const bcUINT32 l_thread_count = std::min(hardware_worker_count(), l_job_count);

			concurrent_for_each
			(
				l_thread_count,
				p_begin,
				p_end,
				[]()
				{
					return true;
				},
				[&p_body_func](bool, typename std::iterator_traits<TIte>::reference p_item)
				{
					p_body_func(p_item);
				},
				[](bool)
				{
				}
			);
		}

		template<typename TIte, typename TBodyFunc>
		void bc_concurrency::concurrent_for_each(bcUINT32 p_thread_count, TIte p_begin, TIte p_end, TBodyFunc p_body_func)
		{
			if(p_thread_count > 1)
			{
				_concurrent_for_each
				(
					p_thread_count,
					p_begin,
					p_end,
					[]()
					{
						return true;
					},
					[&p_body_func](bool, typename std::iterator_traits<TIte>::reference p_item)
					{
						p_body_func(p_item);
					},
					[](bool)
					{
					}
				);
			}
			else
			{
				std::for_each(p_begin, p_end, [&p_body_func](typename std::iterator_traits<TIte>::reference p_item)
				{
					p_body_func(p_item);
				});
			}
		}

		template<typename T>
		T* bc_concurrency::double_check_lock(core_platform::bc_atomic<T*>& p_pointer, core_platform::bc_mutex& p_mutex, const bc_delegate<T*()>& p_initializer)
		{
			T* l_pointer;

			if ((l_pointer = p_pointer.load(core_platform::bc_memory_order::acquire)) == nullptr)
			{
				core_platform::bc_mutex_guard l_guard(p_mutex);

				if ((l_pointer = p_pointer.load(core_platform::bc_memory_order::relaxed)) == nullptr)
				{
					l_pointer = p_initializer();
					p_pointer.store(l_pointer, core_platform::bc_memory_order::release);
				}
			}

			return l_pointer;
		}

		template<typename TIte, typename TInitFunc, typename TBodyFunc, typename TFinalFunc>
		void bc_concurrency::_concurrent_for_each(bcUINT32 p_thread_count, TIte p_begin, TIte p_end, TInitFunc p_init_func, TBodyFunc p_body_func, TFinalFunc p_finalizer_func)
		{
			struct for_each_state
			{
				bc_task<void> m_task;
				TIte m_begin;
				TIte m_end;
				TInitFunc* m_init_func;
				TBodyFunc* m_body_func;
				TFinalFunc* m_finalizer_func;

				for_each_state(TIte p_begin,
					TIte p_end,
					TInitFunc& p_init_func,
					TBodyFunc& p_body_func,
					TFinalFunc& p_finalizer_func)
					: m_begin(p_begin),
					m_end(p_end),
					m_init_func(&p_init_func),
					m_body_func(&p_body_func),
					m_finalizer_func(&p_finalizer_func)
				{
				}

				void wait() const
				{
					m_task.wait();
				}
			};
			
			const bcUINT32 l_thread_count = std::max(p_thread_count, 1U);
			const bcUINT32 l_job_count = std::distance(p_begin, p_end);
			const bcUINT32 l_chunk_size = static_cast<bcUINT32>(std::ceil(static_cast<bcFLOAT>(l_job_count) / static_cast<bcFLOAT>(l_thread_count)));

			bc_vector_frame<for_each_state> l_states;
			l_states.reserve(l_thread_count);

			for (bcUINT32 l_thread = 0; l_thread < l_thread_count; ++l_thread)
			{
				auto l_begin_index = l_thread * l_chunk_size;
				auto l_end_index = (l_thread + 1) * l_chunk_size;
				l_end_index = std::min(l_end_index, l_job_count);

				if (l_begin_index >= l_end_index)
				{
					break;
				}

				l_states.push_back(for_each_state(p_begin, p_begin, p_init_func, p_body_func, p_finalizer_func));
				auto& l_state = l_states.back();
				
				std::advance(l_state.m_begin, l_begin_index);
				std::advance(l_state.m_end, l_end_index);

				l_state.m_task = start_task
				(
					bc_delegate<void()>
					(
						[&l_state]()
						{
							auto l_init = (*l_state.m_init_func)();

							std::for_each
							(
								l_state.m_begin,
								l_state.m_end,
								[&l_state, &l_init](typename std::iterator_traits<TIte>::reference p_item)
								{
									(*l_state.m_body_func)(l_init, p_item);
								}
							);

							(*l_state.m_finalizer_func)(l_init);
						}
					)
				);
			}

			when_all(std::begin(l_states), std::end(l_states));

			try
			{
				for (for_each_state& l_state : l_states)
				{
					// Call get to find if tasks has thrown any exception 
					l_state.m_task.get();
				}
			}
			catch (std::exception & p_exception)
			{
				bc_app_event_error l_error_event(bc_string("Concurrent for_each task exited with error: ") + p_exception.what());
				bc_get_service<bc_event_manager>()->process_event(l_error_event);

				throw;
			}
		}
		
		inline bc_thread_manager* bc_concurrency::_get_thread_manager()
		{
			static auto* s_thread_manager = bc_get_service<bc_thread_manager>();

			return s_thread_manager;
		}
	}
}