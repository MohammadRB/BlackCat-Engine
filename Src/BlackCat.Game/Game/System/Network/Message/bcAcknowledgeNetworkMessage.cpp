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
		bc_acknowledge_network_message::bc_acknowledge_network_message()
			: bci_network_message(message_name()),
			m_ack_id(0)
		{
		}

		bc_acknowledge_network_message::bc_acknowledge_network_message(bc_network_message_id p_id, core::bc_string p_ack_data)
			: bci_network_message(message_name()),
			m_ack_id(p_id),
			m_ack_data(std::move(p_ack_data))
		{
		}

		bc_acknowledge_network_message::bc_acknowledge_network_message(bc_acknowledge_network_message&&) noexcept = default;

		bc_acknowledge_network_message::~bc_acknowledge_network_message() = default;

		bc_acknowledge_network_message& bc_acknowledge_network_message::operator=(bc_acknowledge_network_message&&) noexcept = default;

		void bc_acknowledge_network_message::execute(const bc_network_message_client_context& p_context) noexcept
		{
			p_context.m_visitor.acknowledge_message(get_acknowledged_message_id(), std::move(m_ack_data));
		}

		void bc_acknowledge_network_message::execute(const bc_network_message_server_context& p_context) noexcept
		{
			p_context.m_visitor.acknowledge_message(p_context.m_client.m_address, get_acknowledged_message_id(), std::move(m_ack_data));
		}

		void bc_acknowledge_network_message::serialize_message(const bc_network_message_serialization_context& p_context)
		{
			p_context.m_params.add("ack_id", core::bc_any(m_ack_id));
			p_context.m_params.add("ack_data", core::bc_any(m_ack_data));
		}

		void bc_acknowledge_network_message::deserialize_message(const bc_network_message_deserialization_context& p_context)
		{
			auto* l_ack_id = p_context.m_params.find("ack_id")->second.as<bc_network_message_id>();
			auto* l_ack_data = p_context.m_params.find("ack_data")->second.as<core::bc_string>();
			if (!l_ack_id)
			{
				core::bc_log(core::bc_log_type::error, bcL("Failed to deserialize acknowledge network message id"));
				return;
			}

			m_ack_id = *l_ack_id;
			m_ack_data = std::move(*l_ack_data);
		}
	}	
}