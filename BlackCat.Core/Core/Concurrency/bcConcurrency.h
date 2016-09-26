// [05/14/2016 MRB]

#pragma once

#include "Core/Container/bcIterator.h"
#include "Core/Container/bcVector.h"
#include "Core/Concurrency/bcThreadManager.h"
#include "Core/Event/bcEventManager.h"
#include "Core/bcEvent.h"

namespace black_cat
{
	namespace core
	{
		class bc_concurreny
		{
		public:
			void check_for_interruption();

			template< typename T >
			static bc_task<T> start_task(bc_delegate< T(void) >&& p_delegate, bc_task_creation_option p_option = bc_task_creation_option::none);

			template< typename ...T >
			static void when_all(bc_task<T>&... p_tasks);

			template< typename T >
			static void when_all(std::initializer_list<bc_task<T>&> p_tasks);

			template< typename TIte >
			static void when_all(TIte p_begin, TIte p_end);

			// TIte:								Iterator type
			// p_init_func:	<TLocal(void)>			Function that initialzie local data
			// TBodyFunc:	<TLocal(TLocal, Ite)>	Function that execute as for loop body
			// TFinalFunc	<void(TLocal)>			Function that will be called by all started threads to accumulate results
			template<typename TIte, typename TInitFunc, typename TBodyFunc, typename TFinalFunc>
			static void concurrent_for_each(TIte p_begin, TIte p_end, TInitFunc p_init_func, TBodyFunc p_body_func, TFinalFunc p_finalizer);
			
			template<typename TIte, typename TInitFunc, typename TBodyFunc, typename TFinalFunc>
			static void concurrent_for_each(bcUINT32 p_num_thread, TIte p_begin, TIte p_end, TInitFunc p_init_func, TBodyFunc p_body_func, TFinalFunc p_finalizer);

		protected:

		private:
			static bc_thread_manager* _get_thread_manager()
			{
				static bc_thread_manager* m_thread_manager = bc_service_manager::get().get_service< bc_thread_manager >();

				return m_thread_manager;
			}
		};

		inline void bc_concurreny::check_for_interruption()
		{
			_get_thread_manager()->check_for_interruption();
		}

		template< typename T >
		bc_task<T> bc_concurreny::start_task(bc_delegate<T()>&& p_delegate, bc_task_creation_option p_option)
		{
			return _get_thread_manager()->start_new_task(std::move(p_delegate), p_option);
		}

		template< typename ...T >
		void bc_concurreny::when_all(bc_task<T>&... p_tasks)
		{
			[](...) {} ((p_tasks.wait())...);
		}

		template< typename T >
		void bc_concurreny::when_all(std::initializer_list<bc_task<T>&> p_tasks)
		{
			std::for_each(std::begin(p_tasks), std::end(p_tasks), [](const bc_task<T>& p_task)
			{
				p_task.wait();
			});
		}

		template< typename TIte >
		void bc_concurreny::when_all(TIte p_begin, TIte p_end)
		{
			std::for_each(p_begin, p_end, [](const typename std::iterator_traits< TIte >::value_type& p_task)
			{
				p_task.wait();
			});
		}

		template< typename TIte, typename TInitFunc, typename TBodyFunc, typename TFinalFunc >
		void bc_concurreny::concurrent_for_each(TIte p_begin, TIte p_end, TInitFunc p_init_func, TBodyFunc p_body_func, TFinalFunc p_finalizer)
		{
			concurrent_for_each(_get_thread_manager()->thread_count(), p_begin, p_end, p_init_func, p_body_func, p_finalizer);
		}

		template< typename TIte, typename TInitFunc, typename TBodyFunc, typename TFinalFunc >
		void bc_concurreny::concurrent_for_each(bcUINT32 p_num_thread, TIte p_begin, TIte p_end, TInitFunc p_init_func, TBodyFunc p_body_func, TFinalFunc p_finalizer)
		{
			static_assert
				(
					std::is_same
					<
						typename std::iterator_traits< TIte >::iterator_category,
						std::random_access_iterator_tag
					>::value,
					"Iterator must random access iterator type"
				);

			bcUINT32 l_num_thread = std::max(p_num_thread, 1U);
			bcUINT32 l_num = std::distance(p_begin, p_end);
			bcUINT32 l_chund_size = std::ceil((l_num * 1.f) / l_num_thread);

			bc_vector_frame< bc_task< void > > l_tasks;
			l_tasks.reserve(l_num_thread);

			for (bcUINT32 l_thread = 0; l_thread < l_num_thread; ++l_thread)
			{
				bcUINT32 l_begin_index = l_thread * l_chund_size;
				bcUINT32 l_end_index = (l_thread + 1) * l_chund_size;
				l_end_index = l_end_index > l_num ? l_num : l_end_index;

				if(l_begin_index >= l_end_index)
				{
					break;
				}

				TIte l_begin = p_begin + l_begin_index;
				TIte l_end = p_begin + l_end_index;

				bc_task< void > l_task = start_task
					(
						bc_delegate< void() >::make_from_big_object
						(
							bc_alloc_type::level,
							[l_begin, l_end, &p_init_func, &p_body_func, &p_finalizer]()
							{
								auto l_init = p_init_func();

								std::for_each(l_begin, l_end, [l_begin, l_end, &l_init, &p_body_func](typename std::iterator_traits< TIte >::reference p_item)
									{
										p_body_func(l_init, p_item);
									});

								p_finalizer(l_init);
							}
						),
						bc_task_creation_option::fairness
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
			catch (std::exception& p_exception)
			{
				auto* l_event_manager = bc_service_manager::get().get_service< bc_event_manager >();

				bc_app_event_error l_error_event(p_exception.what());

				l_event_manager->process_event(l_error_event);

				throw p_exception;
			}
#endif
		}
	}
}