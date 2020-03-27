// [2/11/2015 MRB]

#include "Core/CorePCH.h"
#include "Core/Event/bcEventManager.h"

namespace black_cat
{
	namespace core
	{
		bc_event_manager::bc_event_manager()
			: m_local_queue(bc_memory_pool_allocator<_bc_queued_event>(m_local_queue_pool)),
			m_total_elapsed(0)
		{
			m_local_queue_pool.initialize(250, sizeof(decltype(m_local_queue)::node_type), bc_alloc_type::unknown);
		}

		bc_event_manager::~bc_event_manager()
		{
		}

		void bc_event_manager::replace_event_listener(bc_event_listener_handle& p_listener_handle, delegate_type&& p_listener)
		{
			const auto l_hash = bc_ievent::get_hash(p_listener_handle.m_event_name);

			{
				core_platform::bc_lock_guard< core_platform::bc_shared_mutex > l_guard(m_handlers_mutex);

				auto l_ite = m_handlers.find(l_hash);
				bcAssert(l_ite != m_handlers.end());

				l_ite->second.replace_delegate(p_listener_handle.m_event_index, std::move(p_listener));
			}
		}

		void bc_event_manager::unregister_event_listener(bc_event_listener_handle& p_listener_handle)
		{
			const auto l_hash = bc_ievent::get_hash(p_listener_handle.m_event_name);

			{
				core_platform::bc_lock_guard< core_platform::bc_shared_mutex > l_guard(m_handlers_mutex);

				auto l_ite = m_handlers.find(l_hash);
				bcAssert(l_ite != m_handlers.end());

				l_ite->second.remove_delegate(p_listener_handle.m_event_index);

				// If this event handle doesn't has any delegate remove it from container
				if(l_ite->second.delegate_count() == 0)
				{
					m_handlers.erase(l_ite);
				}
			}
		}
		
		bool bc_event_manager::process_event(bc_ievent& p_event)
		{
			const bcUINT32 l_hash = p_event.get_event_hash();
			handler_map_t::const_iterator l_ite;
			bool l_handled = false;

			{
				core_platform::bc_shared_lock< core_platform::bc_shared_mutex > l_guard(m_handlers_mutex);

				l_ite = m_handlers.find(l_hash);
			}

			if (l_ite == m_handlers.end())
			{
				return l_handled;
			}

			l_handled = true;
			l_ite->second(p_event);

			return l_handled;
		}
		
		void bc_event_manager::update(const core_platform::bc_clock::update_param& p_clock_update_param)
		{
			_process_event_queue(p_clock_update_param.m_total_elapsed);
		}

		bc_event_listener_handle bc_event_manager::_register_event_listener(const bcCHAR* p_event_name, delegate_type&& p_listener)
		{
			const auto l_hash = bc_ievent::get_hash(p_event_name);
			bc_event_handler_index l_index;

			{
				core_platform::bc_lock_guard< core_platform::bc_shared_mutex > l_guard(m_handlers_mutex);

				auto& l_event_handle = m_handlers[l_hash];
				l_index = l_event_handle.add_delegate(std::move(p_listener));
			}

			return bc_event_listener_handle(p_event_name, l_index);
		}

		void bc_event_manager::_queue_event(bc_event_ptr<bc_ievent>&& p_event, core_platform::bc_clock::small_delta_time p_millisecond)
		{
			m_global_queue.push(_bc_queued_event(std::move(p_event), p_millisecond));
		}

		bcUINT32 bc_event_manager::_process_event_queue(core_platform::bc_clock::big_clock p_current_time)
		{
			bcUINT32 l_processed_event_count = 0;
			_bc_queued_event l_event(nullptr, 0);

			while (m_global_queue.pop(l_event))
			{
				l_event.m_process_time += m_total_elapsed;

				const auto l_lower_bound = std::lower_bound
				(
					std::cbegin(m_local_queue),
					std::cend(m_local_queue),
					l_event,
					[](const _bc_queued_event& p_first, const _bc_queued_event& p_second)-> bool
					{
						return p_first.m_process_time >= p_second.m_process_time;
					}
				);
				m_local_queue.insert(l_lower_bound, std::move(l_event));
			}

			while (!m_local_queue.empty() && m_local_queue.back().m_process_time <= p_current_time)
			{
				process_event(*m_local_queue.back().m_event);

				m_local_queue.pop_back();
				++l_processed_event_count;
			}

			m_total_elapsed = p_current_time;

			return l_processed_event_count;
		}
	}
}