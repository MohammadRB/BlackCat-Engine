// [03/14/2020 MRB]

#include "Core/CorePCH.h"
#include "Core/Messaging/bcMessage.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace core
	{
		bc_imessage::~bc_imessage() = default;

		const bcCHAR* bc_imessage::get_name() const noexcept
		{
			return m_name;
		}

		bc_message_hash bc_imessage::get_hash() const noexcept
		{
			if (m_hash == 0)
			{
				m_hash = get_hash(m_name);
			}

			return m_hash;
		}

		bc_message_hash bc_imessage::get_hash(const bcCHAR* p_name) noexcept
		{
			const bc_message_hash l_hash = BC_RUN_TIME_STRING_HASH(p_name);

			bcAssert(l_hash != 0);

			return l_hash;
		}

		bc_imessage::bc_imessage(const bcCHAR* p_name)
			: m_name(p_name),
			m_hash(0)
		{
		}

		bc_imessage::bc_imessage(const bc_imessage& p_other)
			: m_name(p_other.m_name),
			m_hash(p_other.m_hash)
		{
		}

		bc_imessage& bc_imessage::operator=(const bc_imessage& p_other)
		{
			m_name = p_other.m_name;
			m_hash = p_other.m_hash;

			return *this;
		}
	}
}