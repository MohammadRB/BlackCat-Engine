// [2/11/2015 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutexProvider.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Event/bcEvent.h"
#include "Core/Event/bcEventHandler.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcConcurrentQueue.h"

namespace black_cat
{
	namespace core
	{
		class BC_COREDLL_EXP bc_event_manager
		{
		public:
			using event_handler_type = bc_event_handler< bool(bc_ievent&) >;
			using delegate_type = event_handler_type::delegate_type;
			using bc_event_ptr = bc_unique_ptr< bc_ievent >;
		
		public:
			bc_event_manager()
			{
			}

			~bc_event_manager()
			{
			}

			void initialize()
			{
			}

			bc_event_handle register_event_listener(const bcCHAR* p_event_name, delegate_type&& p_listener);

			void unregister_event_listener(const bcCHAR* p_event_name, bc_event_handle p_listener_index);

			// Process an event and return true if there is any handler for this type of event, otherwise return false
			bool process_event(bc_ievent& p_event);

			// Queue event for processing in a specific time in future that will be indicated by millisecond.
			// This function is thread safe and can be called with multiple threads.
			void queue_event(bc_event_ptr&& p_event, 
				core_platform::bc_clock::large_time_delta_type p_current_time,
				core_platform::bc_clock::little_time_delta_type p_milisecond);

			// Process queued events if it's time to handle them and return number of proccessed events.
			// This function isn't thread safe and in one time must be called only by one thread.
			bcUINT32 process_event_queue(core_platform::bc_clock::large_time_delta_type p_current_time);

		protected:

		private:
			class _queued_event
			{
			public:
				_queued_event() // This is for bc_concurrent_queue constraint on default construction
					: m_event(nullptr),
					m_process_time(0)
				{
				}

				_queued_event(bc_event_ptr&& p_event, core_platform::bc_clock::large_time_delta_type p_process_time) noexcept(true)
					: m_event(std::move(p_event)),
					m_process_time(p_process_time)
				{
				}

				_queued_event(_queued_event&& p_other) noexcept(true)
					: m_event(std::move(p_other.m_event)),
					m_process_time(p_other.m_process_time)
				{
				}

				~_queued_event() = default;

				_queued_event& operator =(_queued_event&& p_other) noexcept(true)
				{
					m_event = std::move(p_other.m_event);
					m_process_time = p_other.m_process_time;

					return *this;
				}

				bc_event_ptr m_event;
				core_platform::bc_clock::large_time_delta_type m_process_time;
			};

			core_platform::bc_shared_mutex m_mutex;
			bc_unordered_map< bc_event_hash, event_handler_type > m_handlers;

			bc_concurrent_queue< _queued_event > m_global_queue;
			bc_vector< _queued_event > m_local_queue;
		};
	}
}