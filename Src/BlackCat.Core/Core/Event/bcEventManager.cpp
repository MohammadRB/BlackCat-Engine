// [2/11/2015 MRB]

#include "Core/CorePCH.h"
#include "Core/Event/bcEventManager.h"

namespace black_cat
{
	namespace core
	{
		bc_event_manager::bc_event_manager()
			: m_local_queue(bc_memory_pool_allocator<_bc_queued_event>(m_queue_pool)),
			m_render_local_queue(bc_memory_pool_allocator<_bc_queued_event>(m_queue_pool)),
			m_last_elapsed(0),
			m_render_last_elapsed(0)
		{
			m_queue_pool.initialize(250, sizeof(decltype(m_local_queue)::node_type), bc_alloc_type::unknown);
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

				// If this event handle doesn't have any delegate remove it from container
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

		bcUINT32 bc_event_manager::process_event_queue(const core_platform::bc_clock::update_param& p_clock)
		{
			return _process_events_in_queue(p_clock, m_last_elapsed, m_global_queue, m_local_queue);
		}

		bcUINT32 bc_event_manager::process_render_event_queue(const core_platform::bc_clock::update_param& p_clock)
		{
			return _process_events_in_queue(p_clock, m_render_last_elapsed, m_render_global_queue, m_render_local_queue);
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

		bcUINT32 bc_event_manager::_process_events_in_queue(const core_platform::bc_clock::update_param& p_clock,
			core_platform::bc_clock::big_clock& p_last_elapsed,
			bc_concurrent_queue<_bc_queued_event>& p_global_queue,
			bc_list<_bc_queued_event, bc_memory_pool_allocator<_bc_queued_event>>& p_local_queue)
		{
			bcUINT32 l_processed_event_count = 0;
			_bc_queued_event l_event(nullptr, 0);

			while (p_global_queue.pop(l_event))
			{
				l_event.m_process_time += p_last_elapsed;

				const auto l_lower_bound = std::lower_bound
				(
					std::cbegin(p_local_queue),
					std::cend(p_local_queue),
					l_event,
					[](const _bc_queued_event& p_first, const _bc_queued_event& p_second)-> bool
					{
						return p_first.m_process_time >= p_second.m_process_time;
					}
				);
				p_local_queue.insert(l_lower_bound, std::move(l_event));
			}

			while (!p_local_queue.empty() && p_local_queue.back().m_process_time <= p_clock.m_total_elapsed)
			{
				process_event(*p_local_queue.back().m_event);

				p_local_queue.pop_back();
				++l_processed_event_count;
			}

			p_last_elapsed = p_clock.m_total_elapsed;

			return l_processed_event_count;
		}
	}
}