// [10/05/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/System/Network/bcNetworkMessageSerializationBuffer.h"
#include "Game/System/Network/Message/bcPingNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		bc_ping_network_message::bc_ping_network_message()
			: bci_network_message(message_name()),
			m_serialize_time(0)
		{
		}

		bc_ping_network_message::bc_ping_network_message(bc_ping_network_message&&) noexcept = default;

		bc_ping_network_message::~bc_ping_network_message() = default;

		bc_ping_network_message& bc_ping_network_message::operator=(bc_ping_network_message&&) noexcept = default;

		bool bc_ping_network_message::need_acknowledgment() const noexcept
		{
			return true;
		}

		void bc_ping_network_message::acknowledge(const bc_network_message_client_acknowledge_context& p_context) noexcept
		{
			if (get_is_retry())
			{
				// it cannot be a good evaluation of RTT time if message is retried
				return;
			}

			const auto l_elapsed_time = bc_elapsed_packet_time(m_serialize_time);
			p_context.m_visitor.rtt_sample(l_elapsed_time);
		}

		void bc_ping_network_message::acknowledge(const bc_network_message_server_acknowledge_context& p_context) noexcept
		{
			if (get_is_retry())
			{
				// it cannot be a good evaluation of RTT time if message is retried
				return;
			}
			
			const auto l_elapsed_time = bc_elapsed_packet_time(m_serialize_time);
			p_context.m_visitor.rtt_sample(p_context.m_address, l_elapsed_time);
		}

		void bc_ping_network_message::execute(const bc_network_message_client_context& p_context) noexcept
		{
		}

		void bc_ping_network_message::execute(const bc_network_message_server_context& p_context) noexcept
		{
		}

		void bc_ping_network_message::serialize_message(const bc_network_message_serialization_context& p_context)
		{
			m_serialize_time = bc_current_packet_time();
		}

		void bc_ping_network_message::deserialize_message(const bc_network_message_deserialization_context& p_context)
		{
		}
	}	
}