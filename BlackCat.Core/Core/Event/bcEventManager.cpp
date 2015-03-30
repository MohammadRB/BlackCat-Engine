// [2/11/2015 MRB]

#include "Core/CorePCH.h"
#include "Core/Event/bcEventManager.h"

namespace black_cat
{
	namespace core
	{
		bc_event_handle bc_event_manager::register_event_listener(const bcCHAR* p_event_name, delegate_type&& p_listener)
		{
			bcUINT32 l_hash = bc_ievent::get_hash(p_event_name);
			bc_event_handle l_index;

			core_platform::bc_lock_guard< core_platform::bc_shared_mutex > l_gaurd(m_mutex);

			auto& l_event_handle = m_handlers[l_hash];
			l_index = l_event_handle.add_delegate(std::move(p_listener));

			return l_index;
		};

		void bc_event_manager::unregister_event_listener(const bcCHAR* p_event_name, bc_event_handle p_listener_index)
		{
			bcUINT32 l_hash = bc_ievent::get_hash(p_event_name);

			core_platform::bc_lock_guard< core_platform::bc_shared_mutex > l_gaurd(m_mutex);

			auto l_ite = m_handlers.find(l_hash);
			bcAssert(l_ite != m_handlers.end());

			l_ite->second.remove_delegate(p_listener_index);

			// If this event handle doesn't has any delegate remove it from container
			if(l_ite->second.delegate_count() == 0)
				m_handlers.erase(l_ite);
		};
		
		bool bc_event_manager::process_event(bc_ievent& p_event)
		{
			bcUINT32 l_hash = p_event.get_event_hash();
			bool l_handled = false;

			core_platform::bc_shared_lock< core_platform::bc_shared_mutex > l_gaurd(m_mutex);

			auto l_ite = m_handlers.find(l_hash);

			if (l_ite == m_handlers.end())
				return l_handled;

			for (bcUINT32 l_i = 0, l_c = l_ite->second.delegate_count(); l_i < l_c; ++l_i)
			{
				l_handled = true;
				l_ite->second(p_event);
			}

			return l_handled;
		};

		void bc_event_manager::queue_event(bc_event_ptr<bc_ievent>&& p_event,
			core_platform::bc_clock::large_time_delta_type p_current_time,
			core_platform::bc_clock::little_time_delta_type p_milisecond)
		{
			m_global_queue.push(_queued_event(std::move(p_event), p_current_time + p_milisecond));
		};

		bcUINT32 bc_event_manager::process_event_queue(core_platform::bc_clock::large_time_delta_type p_current_time)
		{
			bcUINT32 l_processed_event_count = 0;
			_queued_event l_event(nullptr, 0);

			while (m_global_queue.pop(l_event))
			{
				m_local_queue.push_back(std::move(l_event));
			}
			
			// TODO
			std::sort(
				std::begin(m_local_queue),
				std::end(m_local_queue),
				[](const _queued_event& p_first, const _queued_event& p_second)->bool
				{
					
					return p_first.m_process_time >= p_second.m_process_time;

				});

			while (m_local_queue.size() > 0 && m_local_queue.back().m_process_time <= p_current_time)
			{
				process_event(*m_local_queue.back().m_event);

				m_local_queue.pop_back();
				++l_processed_event_count;
			}

			return l_processed_event_count;
		};
	}
}