// [09/11/2020 MRB]

#include "Core/CorePCH.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Utility/bcCounterValueManager.h"
#include "Core/bcEvent.h"

namespace black_cat::core
{
	bc_counter_value_manager::bc_counter_value_manager()
		: m_read_container(0),
		  m_write_container(1)
	{
		m_swap_frame_event_handle = bc_get_service<bc_event_manager>()->register_event_listener<bc_event_frame_swap>
		(
			bc_event_manager::delegate_type(*this, &bc_counter_value_manager::_event_handler)
		);
	}

	bc_counter_value_manager::~bc_counter_value_manager() = default;

	bc_counter_value_manager::const_iterator bc_counter_value_manager::begin() const noexcept
	{
		return m_container[m_read_container].begin();
	}

	bc_counter_value_manager::const_iterator bc_counter_value_manager::cbegin() const noexcept
	{
		return m_container[m_read_container].cbegin();
	}

	bc_counter_value_manager::const_iterator bc_counter_value_manager::end() const noexcept
	{
		return m_container[m_read_container].end();
	}

	bc_counter_value_manager::const_iterator bc_counter_value_manager::cend() const noexcept
	{
		return m_container[m_read_container].cend();
	}

	bc_counter_value_manager::size_type bc_counter_value_manager::size() const noexcept
	{
		return m_container[m_read_container].size();
	}

	void bc_counter_value_manager::add_counter(const bcCHAR* p_name, bc_wstring p_value)
	{
		m_container[m_write_container].insert_or_assign(p_name, std::move(p_value));
	}

	void bc_counter_value_manager::add_counter(const bcCHAR* p_name, bcFLOAT p_value)
	{
		add_counter(p_name, bc_to_wstring(p_value, L"%.1f"));
	}

	bc_counter_value_manager::const_iterator bc_counter_value_manager::find(const bcCHAR* p_name) const noexcept
	{
		const auto l_ite = m_container[m_read_container].find(p_name);
		if(l_ite == std::cend(m_container[m_read_container]))
		{
			return l_ite;
		}

		if(l_ite->second.empty())
		{
			return std::cend(m_container[m_read_container]);
		}

		return l_ite;
	}

	void bc_counter_value_manager::_event_handler(bci_event& p_event)
	{
		if (bci_message::is<bc_event_frame_swap>(p_event))
		{
			for(auto& [l_key, l_value] : m_container[m_read_container])
			{
				l_value.clear();
			}

			m_read_container = m_write_container;
			m_write_container = (m_read_container + 1) % 2;
				
			return;
		}
	}
}
