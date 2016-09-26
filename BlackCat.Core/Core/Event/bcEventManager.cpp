// [2/11/2015 MRB]

#include "Core/CorePCH.h"
#include "Core/Event/bcEventManager.h"

namespace black_cat
{
	namespace core
	{
		bc_event_listener_handle::bc_event_listener_handle()
			: m_event_name(nullptr),
			m_event_index(-1)
		{
		}

		bc_event_listener_handle::bc_event_listener_handle(const bcCHAR* p_event_name, bcSIZE p_event_index)
			: m_event_name(p_event_name),
			m_event_index(p_event_index)
		{
		}

		bc_event_listener_handle::bc_event_listener_handle(bc_event_listener_handle&& p_other) noexcept
		{
			m_event_name = p_other.m_event_name;
			m_event_index = p_other.m_event_index;
			p_other.m_event_name = nullptr;
			p_other.m_event_index = -1;
		}

		bc_event_listener_handle::~bc_event_listener_handle()
		{
			reset();
		}

		bc_event_listener_handle& bc_event_listener_handle::operator=(bc_event_listener_handle&& p_other) noexcept
		{
			reset(std::move(p_other));

			return *this;
		}

		void bc_event_listener_handle::reset()
		{
			if (m_event_name)
			{
				auto* l_event_manager = bc_service_manager::get().get_service<bc_event_manager>();

				l_event_manager->unregister_event_listener(*this);
			}
		}

		void bc_event_listener_handle::reset(bc_event_listener_handle&& p_other)
		{
			reset();

			m_event_name = p_other.m_event_name;
			m_event_index = p_other.m_event_index;
			p_other.m_event_name = nullptr;
			p_other.m_event_index = -1;
		}

		bc_event_listener_handle bc_event_manager::register_event_listener(const bcCHAR* p_event_name, delegate_type&& p_listener)
		{
			bcUINT32 l_hash = bc_ievent::get_hash(p_event_name);
			bc_event_handler_index l_index;

			core_platform::bc_lock_guard< core_platform::bc_shared_mutex > l_gaurd(m_mutex);

			auto& l_event_handle = m_handlers[l_hash];
			l_index = l_event_handle.add_delegate(std::move(p_listener));

			return bc_event_listener_handle(p_event_name, l_index);
		};

		void bc_event_manager::unregister_event_listener(bc_event_listener_handle& p_listener_handle)
		{
			bcUINT32 l_hash = bc_ievent::get_hash(p_listener_handle.m_event_name);

			core_platform::bc_lock_guard< core_platform::bc_shared_mutex > l_gaurd(m_mutex);

			auto l_ite = m_handlers.find(l_hash);
			bcAssert(l_ite != m_handlers.end());

			l_ite->second.remove_delegate(p_listener_handle.m_event_index);

			// If this event handle doesn't has any delegate remove it from container
			if(l_ite->second.delegate_count() == 0)
				m_handlers.erase(l_ite);

			p_listener_handle.m_event_name = nullptr;
			p_listener_handle.m_event_index = -1;
		};
		
		bool bc_event_manager::process_event(bc_ievent& p_event)
		{
			bcUINT32 l_hash = p_event.get_event_hash();
			handler_map_t::const_iterator l_ite;
			bool l_handled = false;

			{
				core_platform::bc_shared_lock< core_platform::bc_shared_mutex > l_gaurd(m_mutex);

				l_ite = m_handlers.find(l_hash);
			}

			if (l_ite == m_handlers.end())
				return l_handled;

			l_handled = true;
			l_ite->second(p_event);

			return l_handled;
		};

		void bc_event_manager::queue_event(bc_event_ptr<bc_ievent>&& p_event,
			core_platform::bc_clock::big_delta_time p_current_time,
			core_platform::bc_clock::small_delta_time p_milisecond)
		{
			m_global_queue.push(_bc_queued_event(std::move(p_event), p_current_time + p_milisecond));
		};

		bcUINT32 bc_event_manager::process_event_queue(core_platform::bc_clock::big_delta_time p_current_time)
		{
			bcUINT32 l_processed_event_count = 0;
			_bc_queued_event l_event(nullptr, 0);

			while (m_global_queue.pop(l_event))
			{
				m_local_queue.push_back(std::move(l_event));
			}
			
			// TODO
			std::sort(
				std::begin(m_local_queue),
				std::end(m_local_queue),
				[](const _bc_queued_event& p_first, const _bc_queued_event& p_second)->bool
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