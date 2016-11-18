// [2/11/2015 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/bcExport.h"
#include "Core/bcConstant.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Event/bcEvent.h"
#include "Core/Event/bcEventHandler.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcConcurrentQueue.h"
#include "Core/Utility/bcServiceManager.h"

namespace black_cat
{
	namespace core
	{
		template< class TEvent >
		using bc_event_ptr = bc_unique_ptr<TEvent>;

		template< class TEvent >
		bc_event_ptr<TEvent> bc_make_event(TEvent& p_event)
		{
			return bc_make_unique<TEvent>(p_event);
		}

		class _bc_queued_event
		{
		public:
			// This is for bc_concurrent_queue constraint on default construction
			_bc_queued_event() noexcept
				: m_event(nullptr),
				m_process_time(0)
			{
			}

			_bc_queued_event(bc_event_ptr<bc_ievent>&& p_event, core_platform::bc_clock::big_delta_time p_process_time) noexcept(true)
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
			core_platform::bc_clock::big_delta_time m_process_time;
		};

		// Pushing and queueing events are threat safe operations
		class BC_CORE_DLL bc_event_manager : public bc_iservice
		{
			BC_SERVICE(event_manager)

		public:
			using event_handler_type = bc_event_handler< bool(bc_ievent&) >;
			using delegate_type = event_handler_type::delegate_type;

		private:
			using handler_map_t = bc_unordered_map< bc_event_hash, event_handler_type >;
		
		public:
			bc_event_manager();

			~bc_event_manager();

			template< class TEvent >
			bc_event_listener_handle register_event_listener(delegate_type&& p_listener)
			{
				return register_event_listener(bc_event_traits< TEvent >::event_name(), std::move(p_listener));
			}

			bc_event_listener_handle register_event_listener(const bcCHAR* p_event_name, delegate_type&& p_listener);

			void unregister_event_listener(bc_event_listener_handle& p_listener_handle);

			// Process an event and return true if there is any handler for this type of event, otherwise return false
			bool process_event(bc_ievent& p_event);

			// Queue event for processing in a specific time in future that will be indicated by millisecond.
			// This function is thread safe and can be called with multiple threads.
			void queue_event(bc_event_ptr<bc_ievent>&& p_event, core_platform::bc_clock::small_delta_time p_milisecond);

			// Process queued events if it's time to handle them and return number of proccessed events.
			// This function isn't thread safe and in one time must be called only by one thread.
			bcUINT32 process_event_queue(core_platform::bc_clock::big_clock p_current_time);

		protected:
			void update(core_platform::bc_clock::update_param p_clock_update_param) override
			{
				process_event_queue(p_clock_update_param.m_total_elapsed);
			}

		private:
			core_platform::bc_shared_mutex m_handlers_mutex;
			handler_map_t m_handlers;

			bc_vector< _bc_queued_event > m_local_queue;
			bc_concurrent_queue< _bc_queued_event > m_global_queue;
			core_platform::bc_clock::big_clock m_total_elapsed;
		};
	}
}