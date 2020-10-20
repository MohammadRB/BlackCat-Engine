// [05/14/2016 MRB]

#pragma once

#include "Core/Container/bcIterator.h"
#include "Core/Container/bcVector.h"
#include "Core/Concurrency/bcThreadManager.h"
#include "Core/Event/bcEventManager.h"
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

			static bcUINT32 worker_count();

			template< typename T >
			static bc_task<T> start_task(bc_delegate< T(void) >&& p_delegate, bc_task_creation_option p_option = bc_task_creation_option::policy_none);

			template< typename ...T >
			static void when_all(bc_task<T>&... p_tasks);

			template< typename T >
			static void when_all(std::initializer_list<bc_task<T>&> p_tasks);

			template< typename TIte, typename = std::enable_if_t< bc_is_iterator< TIte >::value >>
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
			 * \param p_finalizer 
			 */
			template<typename TIte, typename TInitFunc, typename TBodyFunc, typename TFinalFunc>
			static void concurrent_for_each(TIte p_begin, TIte p_end, TInitFunc p_init_func, TBodyFunc p_body_func, TFinalFunc p_finalizer);
			
			template<typename TIte, typename TInitFunc, typename TBodyFunc, typename TFinalFunc>
			static void concurrent_for_each(bcUINT32 p_num_thread, TIte p_begin, TIte p_end, TInitFunc p_init_func, TBodyFunc p_body_func, TFinalFunc p_finalizer);

			template< typename T >
			static T* double_check_lock(core_platform::bc_atomic< T* >& p_pointer, bc_delegate< T*() >& p_initializer);

		private:
			template<typename TIte, typename TInitFunc, typename TBodyFunc, typename TFinalFunc>
			static void _concurrent_for_each(bcUINT32 p_num_thread, TIte p_begin, TIte p_end, TInitFunc p_init_func, TBodyFunc p_body_func, TFinalFunc p_finalizer);
			
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

		inline bcUINT32 bc_concurrency::worker_count()
		{
			return _get_thread_manager()->spawned_thread_count();
		}

		template< typename T >
		bc_task<T> bc_concurrency::start_task(bc_delegate<T()>&& p_delegate, bc_task_creation_option p_option)
		{
			return _get_thread_manager()->start_new_task(std::move(p_delegate), p_option);
		}

		template< typename ...T >
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

		template< typename T >
		void bc_concurrency::when_all(std::initializer_list<bc_task<T>&> p_tasks)
		{
			std::for_each(std::begin(p_tasks), std::end(p_tasks), [](const bc_task<T>& p_task)
			{
				p_task.wait();
			});
		}

		template< typename TIte, typename >
		void bc_concurrency::when_all(TIte p_begin, TIte p_end)
		{
			std::for_each(p_begin, p_end, [](const typename std::iterator_traits< TIte >::value_type& p_task)
			{
				p_task.wait();
			});
		}

		template< typename TIte, typename TInitFunc, typename TBodyFunc, typename TFinalFunc >
		void bc_concurrency::concurrent_for_each(TIte p_begin, TIte p_end, TInitFunc p_init_func, TBodyFunc p_body_func, TFinalFunc p_finalizer)
		{
			_concurrent_for_each(_get_thread_manager()->spawned_thread_count(), p_begin, p_end, p_init_func, p_body_func, p_finalizer);
		}

		template< typename TIte, typename TInitFunc, typename TBodyFunc, typename TFinalFunc >
		void bc_concurrency::concurrent_for_each(bcUINT32 p_num_thread, TIte p_begin, TIte p_end, TInitFunc p_init_func, TBodyFunc p_body_func, TFinalFunc p_finalizer)
		{
			if(p_num_thread > 1)
			{
				_concurrent_for_each(p_num_thread, p_begin, p_end, p_init_func, p_body_func, p_finalizer);
				return;
			}

			auto l_init = p_init_func();
			std::for_each(p_begin, p_end, [&l_init, &p_body_func](typename std::iterator_traits<TIte>::reference p_item)
			{
				p_body_func(l_init, p_item);
			});
			p_finalizer(l_init);
		}
		
		template< typename T >
		T* bc_concurrency::double_check_lock(core_platform::bc_atomic< T* >& p_pointer, bc_delegate< T*() >& p_initializer)
		{
			T* l_pointer;
			static core_platform::bc_mutex s_mutex; // TODO fix this

			if ((l_pointer = p_pointer.load(core_platform::bc_memory_order::acquire)) == nullptr)
			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(s_mutex);

				if ((l_pointer = p_pointer.load(core_platform::bc_memory_order::relaxed)) == nullptr)
				{
					l_pointer = p_initializer();
					p_pointer.store(l_pointer, core_platform::bc_memory_order::release);
				}
			}

			return l_pointer;
		}

		template< typename TIte, typename TInitFunc, typename TBodyFunc, typename TFinalFunc >
		void bc_concurrency::_concurrent_for_each(bcUINT32 p_num_thread, TIte p_begin, TIte p_end, TInitFunc p_init_func, TBodyFunc p_body_func, TFinalFunc p_finalizer)
		{
			const bcUINT32 l_num_thread = std::max(p_num_thread, 1U);
			const bcUINT32 l_num = std::distance(p_begin, p_end);
			const bcUINT32 l_chunk_size = std::ceil((l_num * 1.f) / l_num_thread);

			bc_vector_frame< bc_task< void > > l_tasks;
			l_tasks.reserve(l_num_thread);

			for (bcUINT32 l_thread = 0; l_thread < l_num_thread; ++l_thread)
			{
				bcUINT32 l_begin_index = l_thread * l_chunk_size;
				bcUINT32 l_end_index = (l_thread + 1) * l_chunk_size;
				l_end_index = std::min(l_end_index, l_num);

				if (l_begin_index >= l_end_index)
				{
					break;
				}

				TIte l_begin = p_begin;
				TIte l_end = p_begin;
				std::advance(l_begin, l_begin_index);
				std::advance(l_end, l_end_index);

				bc_task< void > l_task = start_task
				(
					bc_delegate< void() >::make_from_big_object
					(
						bc_alloc_type::frame,
						[l_begin, l_end, &p_init_func, &p_body_func, &p_finalizer]()
						{
							auto l_init = p_init_func();

							std::for_each
							(
								l_begin,
								l_end,
								[&l_init, &p_body_func](typename std::iterator_traits< TIte >::reference p_item)
								{
									p_body_func(l_init, p_item);
								}
							);

							p_finalizer(l_init);
						}
					),
					bc_task_creation_option::policy_fairness
				);

				l_tasks.push_back(std::move(l_task));
			}

			when_all(std::begin(l_tasks), std::end(l_tasks));

#ifdef BC_DEBUG
			try
			{
#endif
				for (bc_task< void >& l_task : l_tasks) // Call get to find if tasks has thrown any exception 
				{
					l_task.get();
				}
#ifdef BC_DEBUG
			}
			catch (std::exception & p_exception)
			{
				bc_app_event_debug l_error_event(bc_string("Concurrent for_each task exited with error: ") + p_exception.what());
				bc_get_service< bc_event_manager >()->process_event(l_error_event);

				throw;
			}
#endif
		}
		
		inline bc_thread_manager* bc_concurrency::_get_thread_manager()
		{
			static auto* s_thread_manager = bc_get_service< bc_thread_manager >();

			return s_thread_manager;
		}
	}
}