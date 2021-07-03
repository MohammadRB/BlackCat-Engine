// [06/06/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/System/Network/Message/bcAcknowledgeNetworkMessage.h"
#include "Game/System/Network/Server/bcNetworkServerManager.h"
#include "Game/System/Network/Client/bcNetworkClientManager.h"

namespace black_cat
{
	namespace game
	{
		void bc_acknowledge_network_message::execute(const bc_network_message_client_context& p_context) noexcept
		{
			p_context.m_manager.acknowledge_message(get_acknowledged_message_id());
		}

		void bc_acknowledge_network_message::execute(const bc_network_message_server_context& p_context) noexcept
		{
			p_context.m_manager.acknowledge_message(p_context.m_address, get_acknowledged_message_id());
		}

		void bc_acknowledge_network_message::serialize_message(core::bc_json_key_value& p_params) const
		{
			p_params.add(std::make_pair("ack_id", core::bc_any(m_ack_message_id)));
		}

		void bc_acknowledge_network_message::deserialize_message(const core::bc_json_key_value& p_params)
		{
			auto* l_ack_id = p_params.find("ack_id")->second.as<bc_network_message_id>();
			if (!l_ack_id)
			{
				core::bc_log(core::bc_log_type::error, bcL("Failed to deserialize acknowledge network message id"));
				return;
			}

			m_ack_message_id = *l_ack_id;
		}
	}	
}