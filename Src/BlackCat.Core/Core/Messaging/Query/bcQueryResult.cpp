// [09/04/2022 MRB]

#include "Core/CorePCH.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "Core/Messaging/Query/bcQueryManager.h"

namespace black_cat::core
{
	bc_query_result::bc_query_result(bc_query_manager& p_query_manager, _bc_query_shared_state& p_shared_state) noexcept
		: m_query_manager(&p_query_manager),
		  m_shared_state(&p_shared_state)
	{
	}

	bc_query_result::bc_query_result() noexcept
		: m_query_manager(nullptr),
		  m_shared_state(nullptr)
	{
	}

	bc_query_result::bc_query_result(bc_query_result&& p_other) noexcept
		: m_query_manager(nullptr),
		  m_shared_state(nullptr)
	{
		operator=(std::move(p_other));
	}

	bc_query_result::~bc_query_result()
	{
		if (m_shared_state)
		{
			m_query_manager->_mark_shared_state(*m_shared_state);
		}
	}

	bc_query_result& bc_query_result::operator=(bc_query_result&& p_other) noexcept
	{
		if (m_shared_state)
		{
			m_query_manager->_mark_shared_state(*m_shared_state);
		}

		m_query_manager = p_other.m_query_manager;
		m_shared_state = p_other.m_shared_state;
		p_other.m_query_manager = nullptr;
		p_other.m_shared_state = nullptr;

		return *this;
	}

	bool bc_query_result::is_executed() const noexcept
	{
		// use acquire memory order so memory changes become available for calling thread
		return m_shared_state && m_shared_state->m_state.load(platform::bc_memory_order::acquire) == _bc_query_shared_state::state::executed;
	}

	bci_query& bc_query_result::get()
	{
		const bool l_is_executed = is_executed();

		if (!l_is_executed)
		{
			BC_ASSERT(false);
			throw bc_invalid_operation_exception("Query is not ready.");
		}

		bci_query& l_query = *m_shared_state->m_query;

		m_query_manager->_mark_shared_state(*m_shared_state);
		m_shared_state = nullptr;

		return l_query;
	}
}
