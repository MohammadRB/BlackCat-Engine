//  [1/24/2015 MRB]

#pragma once

#include "CorePlatform/Concurrency/bcConcurrencyDef.h"
#include "CorePlatformImp/Concurrency/bcConditionVariable.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Concurrency/bcThread.h"
#include "CorePlatformImp/Concurrency/bcThreadLocal.h"
#include "Core/CorePCH.h"
#include "Core/bcExport.h"
#include "Core/bcConstant.h"
#include "Core/bcException.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcConcurrentQueue.h"
#include "Core/Container/bcDeque.h"
#include "Core/Concurrency/bcTask.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcDelegate.hpp"
#include "Core/Utility/bcEnumOperand.h"

namespace black_cat
{
	namespace core
	{
		class BC_CORE_DLL bc_task_stealing_queue
		{
		public:
			using this_type = bc_task_stealing_queue;
			using delegate_type = bc_delegate<void(core_platform::bc_thread::id)>;

		public:
			bc_task_stealing_queue() = default;

			bc_task_stealing_queue(const this_type&) = delete;

			bc_task_stealing_queue(this_type&&) = delete;

			~bc_task_stealing_queue() = default;

			this_type& operator =(const this_type&) = delete;

			this_type& operator =(this_type&&) = delete;

			void push(delegate_type&& p_task);

			bool empty() const;

			bool pop(delegate_type& p_task);

			bool try_steal(delegate_type& p_task);

		private:
			bc_deque< delegate_type > m_deque;
			mutable core_platform::bc_mutex m_mutex;
		};

		struct bc_interrupt_flag
		{
		public:
			bc_interrupt_flag();

			bc_interrupt_flag(bool pFlag);

			bc_interrupt_flag(const bc_interrupt_flag& p_other) = delete;

			bc_interrupt_flag(bc_interrupt_flag&& p_other) = delete;

			~bc_interrupt_flag();

			bc_interrupt_flag& operator =(const bc_interrupt_flag& p_other) = delete;

			bc_interrupt_flag& operator =(bc_interrupt_flag&& p_other) = delete;

			bool test_and_set();

			void clear();

		private:
			core_platform::bc_atomic_flag m_flag;
		};

		enum class bc_task_creation_option : bcUBYTE
		{
			policy_none = bc_enum::value(0),					// Push task in local task queue of current thread
			policy_fairness = bc_enum::value(1),				// Push task in global task queue
			lifetime_exceed_frame = bc_enum::value(2)			// Lifetime of task exceed frame
		};

		class BC_CORE_DLL bc_thread_manager : public bci_service
		{
			BC_SERVICE(trd_mng)

		private:
			using task_type = bc_delegate<void(core_platform::bc_thread::id)>;
			class _thread_data;

		public:
			bc_thread_manager(bcSIZE p_thread_count, bcSIZE p_reserved_thread_count) noexcept;

			bc_thread_manager(const bc_thread_manager&) = delete;

			bc_thread_manager(bc_thread_manager&& p_other) noexcept = delete;

			~bc_thread_manager();

			bc_thread_manager& operator=(const bc_thread_manager&) = delete;

			bc_thread_manager& operator=(bc_thread_manager&& p_other) noexcept = delete;

			bcSIZE max_thread_count() const;

			bcSIZE spawned_thread_count() const;

			bcSIZE task_count() const;

			void interrupt_thread(core_platform::bc_thread::id p_thread_id);

			void check_for_interruption();

			template<typename T>
			bc_task< T > start_new_task(bc_delegate< T() >&& p_delegate, bc_task_creation_option p_option = bc_task_creation_option::policy_none);

		private:
			void _initialize(bcSIZE p_thread_count, bcSIZE p_reserved_thread_count);

			void _restart_workers();

			void _stop_workers();

			void _push_worker();

			void _join_workers();

			bool _pop_task_from_local_queue(task_type& p_task);

			bool _pop_task_from_global_queue(task_type& p_task);

			bool _pop_task_from_others_queue(task_type& p_task);

			void _worker_spin(bcUINT32 p_my_index);

			static constexpr bcSIZE s_worker_switch_threshold = 20;
			static constexpr bcSIZE s_new_thread_threshold = 5;
			static constexpr bcSIZE s_num_thread_in_spin = 0;

			bcSIZE m_thread_count;
			bcSIZE m_reserved_thread_count;
			core_platform::bc_atomic< bool > m_done;
			core_platform::bc_atomic< bcUINT32 > m_task_count;
			core_platform::bc_atomic< bcUINT32 > m_num_thread_in_spin;
			core_platform::bc_mutex m_cvariable_mutex;
			core_platform::bc_condition_variable m_cvariable;

			mutable core_platform::bc_shared_mutex m_threads_mutex;
			bc_vector_program< bc_unique_ptr<_thread_data> > m_threads;
			bc_concurrent_queue< task_type > m_global_queue;
			core_platform::bc_thread_local< _thread_data > m_my_data;
		};

		class bc_thread_manager::_thread_data
		{
			friend class bc_thread_manager;

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

		private:
			bcUINT32 m_my_index;
			bc_task_stealing_queue m_local_queue;
			bc_interrupt_flag m_interrupt_flag;
			core_platform::bc_thread m_thread;
		};

		template< typename T >
		bc_task< T > bc_thread_manager::start_new_task(bc_delegate< T() >&& p_delegate, bc_task_creation_option p_option)
		{
			bc_task_link< T > l_task_link(std::move(p_delegate));
			bc_task< T > l_task = l_task_link.get_task();

			bc_alloc_type l_alloc_type = bc_alloc_type::frame;
			bool l_policy_none = true;

			if (bc_enum::has(p_option, bc_task_creation_option::lifetime_exceed_frame))
			{
				l_alloc_type = bc_alloc_type::unknown;
			}

			if (bc_enum::has(p_option, bc_task_creation_option::policy_fairness))
			{
				l_policy_none = false;
			}

			task_type l_task_wrapper(task_type::make_from_big_object(l_alloc_type, std::move(l_task_link))); // TODO
			_thread_data* l_my_data = m_my_data.get();

			const auto l_task_count = m_task_count.fetch_add(1, core_platform::bc_memory_order::seqcst) + 1;

			// If current thread is not main thread
			if (l_my_data && l_policy_none)
			{
				l_my_data->local_queue().push(std::move(l_task_wrapper));
			}
			else
			{
				m_global_queue.push(std::move(l_task_wrapper));
			}

			if(s_num_thread_in_spin == 0)		// If number of steady threads in work spin method is higher than zero
			{									// there is no need to notify a thread
				m_cvariable.notify_one();
			}

			if (l_task_count >= s_new_thread_threshold)
			{
				_push_worker();
			}

			return l_task;
		}
	}
}