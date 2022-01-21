// [07/02/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Network/Message/bcClientConnectNetworkMessage.h"
#include "Game/System/Network/Message/bcAcknowledgeNetworkMessage.h"
#include "Game/bcJsonParse.h"

namespace black_cat
{
	namespace game
	{
		bc_client_connect_network_message::bc_client_connect_network_message()
			: bci_network_message(message_name())
		{
		}

		bc_client_connect_network_message::bc_client_connect_network_message(core::bc_string p_client_name)
			: bci_network_message(message_name()),
			m_client_name(std::move(p_client_name))
		{
		}

		bc_client_connect_network_message::bc_client_connect_network_message(bc_client_connect_network_message&&) noexcept = default;

		bc_client_connect_network_message::~bc_client_connect_network_message() = default;

		bc_client_connect_network_message& bc_client_connect_network_message::operator=(bc_client_connect_network_message&&) noexcept = default;

		core::bc_string bc_client_connect_network_message::get_acknowledgment_data() const noexcept
		{
			return m_error_message;
		}

		void bc_client_connect_network_message::acknowledge(const bc_network_message_client_acknowledge_context& p_context) noexcept
		{
			p_context.m_visitor.connection_approved(p_context.m_ack_data);
		}

		void bc_client_connect_network_message::execute(const bc_network_message_server_context& p_context) noexcept
		{
			m_error_message = p_context.m_visitor.client_connected(p_context.m_address, m_client_name);
		}

		void bc_client_connect_network_message::serialize_message(const bc_network_message_serialization_context& p_context)
		{
			json_parse::bc_write(p_context.m_params, "nme", m_client_name);
		}

		void bc_client_connect_network_message::deserialize_message(const bc_network_message_deserialization_context& p_context)
		{
			json_parse::bc_load(p_context.m_params, "nme", m_client_name);
		}

		void bc_client_disconnect_network_message::execute(const bc_network_message_server_context& p_context) noexcept
		{
			p_context.m_visitor.client_disconnected(p_context.m_address);
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