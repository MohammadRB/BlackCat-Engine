// [07/02/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Network/Message/bcClientConnectNetworkMessage.h"
#include "Game/System/Network/Message/bcAcknowledgeNetworkMessage.h"
#include "Game/System/Network/Client/bcNetworkClientManager.h"
#include "Game/System/Network/Server/bcNetworkServerManager.h"

namespace black_cat
{
	namespace game
	{
		void bc_client_connect_network_message::acknowledge(const bc_network_message_client_context& p_context) noexcept
		{
			p_context.m_manager.connection_approved();
		}

		void bc_client_connect_network_message::execute(const bc_network_message_server_context& p_context) noexcept
		{
			p_context.m_manager.client_connected(p_context.m_address);
		}

		void bc_client_connect_network_message::serialize_message(core::bc_json_key_value& p_params) const
		{
		}

		void bc_client_connect_network_message::deserialize_message(const core::bc_json_key_value& p_params)
		{
		}

		void bc_client_disconnect_network_message::execute(const bc_network_message_server_context& p_context) noexcept
		{
			p_context.m_manager.client_disconnected(p_context.m_address);
		}
		
		void bc_client_disconnect_network_message::serialize_message(core::bc_json_key_value& p_params) const
		{
		}

		void bc_client_disconnect_network_message::deserialize_message(const core::bc_json_key_value& p_params)
		{
		}
	}
}