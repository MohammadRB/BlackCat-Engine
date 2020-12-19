// [2/11/2015 MRB]

#include "Core/CorePCH.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Messaging/Event/bcEventListenerHandle.h"

namespace black_cat
{
	namespace core
	{
		bc_event_listener_handle::bc_event_listener_handle()
			: m_event_name(nullptr),
			m_event_index(-1)
		{
		}

		bc_event_listener_handle::bc_event_listener_handle(bc_event_listener_handle&& p_other) noexcept
			: m_event_name(p_other.m_event_name),
			m_event_index(p_other.m_event_index)
		{
			p_other.m_event_name = nullptr;
			p_other.m_event_index = -1;
		}

		bc_event_listener_handle::bc_event_listener_handle(const bcCHAR* p_event_name, bcSIZE p_event_index)
			: m_event_name(p_event_name),
			m_event_index(p_event_index)
		{
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

		void bc_event_listener_handle::reassign(delegate_type&& p_delegate)
		{
			bc_get_service< bc_event_manager >()->replace_event_listener(*this, std::move(p_delegate));
		}

		void bc_event_listener_handle::reset()
		{
			if (m_event_name)
			{
				bc_get_service< bc_event_manager >()->unregister_event_listener(*this);
				m_event_name = nullptr;
				m_event_index = -1;
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
	}
}