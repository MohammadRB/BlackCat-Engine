// [05/31/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Network/Command/bcNetworkCommand.h"

namespace black_cat
{
	namespace game
	{
		const bcCHAR* bci_network_command::get_command_name() const noexcept
		{
			return m_name;
		}

		bc_network_command_hash bci_network_command::get_command_hash() const noexcept
		{
			if (m_hash == 0)
			{
				m_hash = BC_RUN_TIME_STRING_HASH(m_name);
			}

			return m_hash;
		}

		void bci_network_command::serialize(core::bc_json_key_value& p_params) const
		{
			serialize_command(p_params);
		}

		void bci_network_command::deserialize(const core::bc_json_key_value& p_params)
		{
			deserialize_command(p_params);
		}

		bci_network_command::bci_network_command(const bcCHAR* p_name) noexcept
			: m_name(p_name),
			m_hash(0)
		{
		}
	}	
}