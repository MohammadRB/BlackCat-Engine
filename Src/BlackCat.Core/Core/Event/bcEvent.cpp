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

		bc_event_hash bc_ievent::get_hash(const bcCHAR* p_name) noexcept
		{
			bc_event_hash l_hash = std::hash<const bcCHAR*>()(p_name);

			bcAssert(l_hash != 0);

			return l_hash;
		}

		bc_event::bc_event(const bcCHAR* p_name) noexcept
			: m_name(p_name),
			m_hash(0)
		{
		}

		bc_event::~bc_event()
		{
		}

		const bcCHAR* bc_event::get_event_name() const noexcept
		{
			return m_name;
		}

		bc_event_hash bc_event::get_event_hash() const noexcept
		{
			if (m_hash == 0)
			{
				m_hash = get_hash(m_name);
			}

			return m_hash;
		}

		bc_app_event::bc_app_event(const bcCHAR* p_name) noexcept
			: bc_event(p_name)
		{
		}

		bc_app_event::~bc_app_event()
		{
		}
	}
}