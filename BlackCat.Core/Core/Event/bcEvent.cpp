// [2/11/2015 MRB]

#include "Core/CorePCH.h"
#include "Core/Event/bcEvent.h"
#include "Core/Event/bcEventManager.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace core
	{
		bc_ievent::~bc_ievent()
		{		
		}

		bc_event_hash bc_ievent::get_hash(const bcCHAR* p_name) noexcept(true)
		{
			bc_event_hash l_hash = std::hash<const bcCHAR*>()(p_name);

			bcAssert(l_hash != 0);

			return l_hash;
		}

		bc_event::bc_event(const bcCHAR* p_name) noexcept(true)
			: m_name(p_name),
			m_hash(0)
		{
		}

		bc_event::~bc_event()
		{
		}

		const bcCHAR* bc_event::get_event_name() const noexcept(true)
		{
			return m_name;
		}

		bc_event_hash bc_event::get_event_hash() const noexcept(true)
		{
			if (m_hash == 0)
			{
				m_hash = get_hash(m_name);
			}

			return m_hash;
		}

		bc_app_event::bc_app_event(const bcCHAR* p_name) noexcept(true)
			: bc_event(p_name)
		{
		}

		bc_app_event::~bc_app_event()
		{
		}

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
				auto* l_event_manager = bc_get_service< bc_event_manager >();

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
	}
}