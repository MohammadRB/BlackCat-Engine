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
		bc_client_connect_network_message::bc_client_connect_network_message()
			: bci_network_message(message_name())
		{
		}

		bc_client_connect_network_message::bc_client_connect_network_message(bc_client_connect_network_message&&) noexcept = default;

		bc_client_connect_network_message::~bc_client_connect_network_message() = default;

		bc_client_connect_network_message& bc_client_connect_network_message::operator=(bc_client_connect_network_message&&) noexcept = default;
		
		void bc_client_connect_network_message::acknowledge(const bc_network_message_client_context& p_context) noexcept
		{
			p_context.m_bridge.connection_approved();
		}

		void bc_client_connect_network_message::execute(const bc_network_message_server_context& p_context) noexcept
		{
			p_context.m_bridge.client_connected(p_context.m_address);
		}

		void bc_client_connect_network_message::serialize_message(const bc_network_message_serialization_context& p_context)
		{
		}

		void bc_client_connect_network_message::deserialize_message(const bc_network_message_deserialization_context& p_context)
		{
		}

		void bc_client_disconnect_network_message::execute(const bc_network_message_server_context& p_context) noexcept
		{
			p_context.m_bridge.client_disconnected(p_context.m_address);
		}
		
		void bc_client_disconnect_network_message::serialize_message(const bc_network_message_serialization_context& p_context)
		{
		}

		void bc_client_disconnect_network_message::deserialize_message(const bc_network_message_deserialization_context& p_context)
		{
		}

		bc_client_disconnect_network_message::bc_client_disconnect_network_message()
			: bci_network_message(message_name())
		{
		}

		bc_client_disconnect_network_message::bc_client_disconnect_network_message(bc_client_disconnect_network_message&&) noexcept = default;

		bc_client_disconnect_network_message::~bc_client_disconnect_network_message() = default;

		bc_client_disconnect_network_message& bc_client_disconnect_network_message::operator=(bc_client_disconnect_network_message&&) noexcept = default;
	}
}