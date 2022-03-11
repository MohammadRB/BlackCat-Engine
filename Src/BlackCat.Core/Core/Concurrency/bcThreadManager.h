//  [1/24/2015 MRB]

#pragma once

#include "CorePlatform/Concurrency/bcConcurrencyDef.h"
#include "CorePlatformImp/Concurrency/bcConditionVariable.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Concurrency/bcThread.h"
#include "CorePlatformImp/Concurrency/bcThreadLocal.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcConcurrentQueue.h"
#include "Core/Container/bcDeque.h"
#include "Core/Concurrency/bcTask.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcDelegate.hpp"
#include "Core/Utility/bcEnumOperand.h"
#include "Core/Utility/bcObjectPool.h"
#include "Core/bcExport.h"
#include "Core/bcConstant.h"

namespace black_cat
{
	namespace core
	{
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
			platform::bc_atomic_flag m_flag;
		};

		enum class bc_task_creation_option : bcUBYTE
		{
			policy_none = bc_enum::value(0),
			lifetime_exceed_frame = bc_enum::value(1)
		};

		class BC_CORE_DLL bc_thread_manager : public bci_service
		{
			BC_SERVICE(trd_mng)

		private:
			using task_wrapper_type = bc_delegate<void()>;
			class _thread_data;

		public:
			bc_thread_manager(bcUINT32 p_hardware_thread_count, bcUINT32 p_reserved_thread_count) noexcept;

			bc_thread_manager(const bc_thread_manager&) = delete;

			bc_thread_manager(bc_thread_manager&& p_other) noexcept = delete;

			~bc_thread_manager() override;

			bc_thread_manager& operator=(const bc_thread_manager&) = delete;

			bc_thread_manager& operator=(bc_thread_manager&& p_other) noexcept = delete;

			bcUINT32 hardware_thread_count() const;

			bcUINT32 max_thread_count() const;

			bcUINT32 spawned_thread_count() const;

			bcUINT32 task_count() const;

			void interrupt_thread(platform::bc_thread::id p_thread_id);

			void check_for_interruption();

			template<typename T>
			bc_task<T> start_new_task(bc_delegate<T()> p_delegate, bc_task_creation_option p_option = bc_task_creation_option::policy_none);

		private:
			void _initialize(bcSIZE p_hardware_thread_count, bcSIZE p_reserved_thread_count);

			void _restart_workers();

			void _stop_workers();

			void _push_worker();

			void _join_workers();

			bool _pop_task_from_global_queue(task_wrapper_type& p_task);

			void _worker_spin(bcUINT32 p_my_index);

			static constexpr bcSIZE s_worker_switch_threshold = 20;
			static constexpr bcSIZE s_new_thread_threshold = 5;
			static constexpr bcSIZE s_num_thread_in_spin = 0;

			bcUINT32 m_hardware_thread_count;
			bcUINT32 m_reserved_thread_count;
			platform::bc_atomic<bool> m_done;
			platform::bc_atomic<bcUINT32> m_spawned_thread_count;
			platform::bc_atomic<bcUINT32> m_num_thread_in_spin;
			platform::bc_atomic<bcUINT32> m_task_count;
			platform::bc_mutex m_cvariable_mutex;
			platform::bc_condition_variable m_cvariable;

			mutable platform::bc_shared_mutex m_threads_mutex;
			bc_vector_program<bc_unique_ptr<_thread_data>> m_threads;
			bc_concurrent_queue<task_wrapper_type> m_global_queue;
			platform::bc_thread_local<_thread_data> m_my_data;
			bc_concurrent_memory_pool m_tasks_pool;
		};

		class bc_thread_manager::_thread_data
		{
			friend class bc_thread_manager;

		public:
			_thread_data(bcUINT32 p_my_index, platform::bc_thread&& p_thread)
				: m_my_index(p_my_index),
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

			bc_interrupt_flag& interrupt_flag()
			{
				return m_interrupt_flag;
			}

			platform::bc_thread& thread()
			{
				return m_thread;
			}

		private:
			bcUINT32 m_my_index;
			bc_interrupt_flag m_interrupt_flag;
			platform::bc_thread m_thread;
		};

		template<typename T>
		bc_task<T> bc_thread_manager::start_new_task(bc_delegate<T()> p_delegate, bc_task_creation_option p_option)
		{
			bc_task_link<T> l_task_link(std::move(p_delegate));
			bc_task<T> l_task = l_task_link.get_task();
			bc_alloc_type l_alloc_type = bc_alloc_type::frame;

			if (bc_enum::has(p_option, bc_task_creation_option::lifetime_exceed_frame))
			{
				l_alloc_type = bc_alloc_type::unknown;
			}

			bool l_task_allocated_from_pool;
			auto* l_allocated_task_link = static_cast<bc_task_link<T>*>(m_tasks_pool.alloc());

			if(l_allocated_task_link)
			{
				l_task_allocated_from_pool = true;
				new (l_allocated_task_link) bc_task_link<T>(std::move(l_task_link));
			}
			else
			{
				l_task_allocated_from_pool = false;
				l_allocated_task_link = BC_NEW(bc_task_link<T>(std::move(l_task_link)), l_alloc_type);
			}

			task_wrapper_type l_task_wrapper([=]() mutable
			{
				(*l_allocated_task_link)();

				if(l_task_allocated_from_pool)
				{
					l_allocated_task_link->~bc_task_link();
					m_tasks_pool.free(l_allocated_task_link);
				}
				else
				{
					BC_DELETE(l_allocated_task_link);
				}
			});

			m_global_queue.push(std::move(l_task_wrapper));
			const auto l_task_count = m_task_count.fetch_add(1, platform::bc_memory_order::relaxed) + 1;

			// If number of steady threads in work spin method is higher than zero there is no need to notify a thread
			if constexpr (s_num_thread_in_spin == 0)
			{
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