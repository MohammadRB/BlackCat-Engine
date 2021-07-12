// [05/31/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		core_platform::bc_atomic<bc_network_message_id> bci_network_message::s_id_counter(0);

		void bci_network_message::serialize(const bc_network_message_serialization_context& p_context)
		{
			m_id = s_id_counter.fetch_add(1);
			p_context.m_params.add("id", core::bc_any(m_id));
			serialize_message(p_context);
		}

		void bci_network_message::deserialize(const bc_network_message_deserialization_context& p_context)
		{
			auto* l_id = p_context.m_params.find("id")->second.as<bc_network_message_id>();
			if(!l_id)
			{
				core::bc_log(core::bc_log_type::error, bcL("Failed to deserialize network message id"));
				return;
			}

			m_id = *l_id;
			deserialize_message(p_context);
		}

		void bci_network_message::execute(const bc_network_message_client_context& p_context) noexcept
		{
		}

		void bci_network_message::execute(const bc_network_message_server_context& p_context) noexcept
		{
		}

		void bci_network_message::acknowledge(const bc_network_message_client_context& p_context) noexcept
		{
		}

		void bci_network_message::acknowledge(const bc_network_message_server_context& p_context) noexcept
		{
		}
	}
}