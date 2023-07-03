// [14/03/2020 MRB]

#include "Core/CorePCH.h"
#include "Core/Messaging/bcMessage.h"

namespace black_cat
{
	namespace core
	{
		bci_message::~bci_message() = default;

		const bcCHAR* bci_message::get_message_name() const noexcept
		{
			return m_name;
		}

		bc_message_hash bci_message::get_message_hash() const noexcept
		{
			if (m_hash == 0)
			{
				m_hash = get_hash(m_name);
			}

			return m_hash;
		}

		bc_message_hash bci_message::get_hash(const bcCHAR* p_name) noexcept
		{
			const bc_message_hash l_hash = BC_RUN_TIME_STRING_HASH(p_name);

			BC_ASSERT(l_hash != 0);

			return l_hash;
		}

		bci_message::bci_message(const bcCHAR* p_name) noexcept
			: m_name(p_name),
			m_hash(0)
		{
		}

		bci_message::bci_message(const bci_message& p_other) noexcept
			: m_name(p_other.m_name),
			m_hash(p_other.m_hash)
		{
		}

		bci_message& bci_message::operator=(const bci_message& p_other) noexcept
		{
			m_name = p_other.m_name;
			m_hash = p_other.m_hash;

			return *this;
		}
	}
}