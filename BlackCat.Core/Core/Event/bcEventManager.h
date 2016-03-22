// [2/11/2015 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Event/bcEvent.h"
#include "Core/Event/bcEventHandler.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcConcurrentQueue.h"
#include "Core/Container/bcString.h"
#include "Core/Utility/bcServiceManager.h"

namespace black_cat
{
	namespace core
	{
		template< class TEvent >
		using bc_event_ptr = bc_unique_ptr<TEvent>;

		class _bc_queued_event
		{
		public:
			// This is for bc_concurrent_queue constraint on default construction
			_bc_queued_event() noexcept
				: m_event(nullptr),
				m_process_time(0)
			{
			}

			_bc_queued_event(bc_event_ptr<bc_ievent>&& p_event, core_platform::bc_clock::large_time_delta_type p_process_time) noexcept(true)
				: m_event(std::move(p_event)),
				m_process_time(p_process_time)
			{
			}

			_bc_queued_event(_bc_queued_event&& p_other) noexcept(true)
				: m_event(std::move(p_other.m_event)),
				m_process_time(p_other.m_process_time)
			{
			}

			~_bc_queued_event() = default;

			_bc_queued_event& operator =(_bc_queued_event&& p_other) noexcept(true)
			{
				m_event = std::move(p_other.m_event);
				m_process_time = p_other.m_process_time;

				return *this;
			}

			bc_event_ptr<bc_ievent> m_event;
			core_platform::bc_clock::large_time_delta_type m_process_time;
		};

		class BC_COREDLL_EXP bc_event_manager : public bc_iservice
		{
		public:
			using event_handler_type = bc_event_handler< bool(bc_ievent&) >;
			using delegate_type = event_handler_type::delegate_type;
		
		public:
			bc_event_manager()
			{
			}

			~bc_event_manager()
			{
			}

			bc_event_handle register_event_listener(const bcCHAR* p_event_name, delegate_type&& p_listener);

			void unregister_event_listener(const bcCHAR* p_event_name, bc_event_handle p_listener_index);

			// Process an event and return true if there is any handler for this type of event, otherwise return false
			bool process_event(bc_ievent& p_event);

			// Queue event for processing in a specific time in future that will be indicated by millisecond.
			// This function is thread safe and can be called with multiple threads.
			void queue_event(bc_event_ptr<bc_ievent>&& p_event, 
				core_platform::bc_clock::large_time_delta_type p_current_time,
				core_platform::bc_clock::little_time_delta_type p_milisecond);

			// Process queued events if it's time to handle them and return number of proccessed events.
			// This function isn't thread safe and in one time must be called only by one thread.
			bcUINT32 process_event_queue(core_platform::bc_clock::large_time_delta_type p_current_time);

			static bc_string service_name()
			{
				return "Event_Manager";
			}

		protected:

		private:
			core_platform::bc_shared_mutex m_mutex;
			bc_unordered_map< bc_event_hash, event_handler_type > m_handlers;

			bc_concurrent_queue< _bc_queued_event > m_global_queue;
			bc_vector< _bc_queued_event > m_local_queue;
		};
	}
}