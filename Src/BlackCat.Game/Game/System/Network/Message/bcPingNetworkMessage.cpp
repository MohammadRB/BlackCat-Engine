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
			m_serialize_time(0),
			m_remote_host_rtt(0)
		{
		}

		bc_ping_network_message::bc_ping_network_message(bc_ping_network_message&&) noexcept = default;

		bc_ping_network_message::~bc_ping_network_message() = default;

		bc_ping_network_message& bc_ping_network_message::operator=(bc_ping_network_message&&) noexcept = default;

		bool bc_ping_network_message::need_acknowledgment() const noexcept
		{
			return true;
		}

		core::bc_string bc_ping_network_message::get_acknowledgment_data() const noexcept
		{
			return core::bc_to_string(m_remote_host_rtt);
		}

		void bc_ping_network_message::acknowledge(const bc_network_message_client_acknowledge_context& p_context) noexcept
		{
			if (get_is_retry())
			{
				// it cannot be a good evaluation of RTT if message is retried
				return;
			}

			const auto l_remote_rtt = core::bc_stof(p_context.m_ack_data);
			const auto l_elapsed_time = bc_elapsed_packet_time(m_serialize_time);
			p_context.m_visitor.add_rtt_sample(l_elapsed_time, l_remote_rtt);
		}

		void bc_ping_network_message::acknowledge(const bc_network_message_server_acknowledge_context& p_context) noexcept
		{
			if (get_is_retry())
			{
				// it cannot be a good evaluation of RTT if message is retried
				return;
			}

			const auto l_remote_rtt = core::bc_stof(p_context.m_ack_data);
			const auto l_elapsed_time = bc_elapsed_packet_time(m_serialize_time);
			p_context.m_visitor.add_rtt_sample(p_context.m_address, l_elapsed_time, l_remote_rtt);
		}

		void bc_ping_network_message::execute(const bc_network_message_client_context& p_context) noexcept
		{
			bc_network_rtt l_rtt;
			bc_network_rtt l_remote_rtt;
			p_context.m_visitor.get_rtt_time(&l_rtt, &l_remote_rtt);

			m_remote_host_rtt = l_rtt;
		}

		void bc_ping_network_message::execute(const bc_network_message_server_context& p_context) noexcept
		{
			bc_network_rtt l_rtt;
			bc_network_rtt l_remote_rtt;
			p_context.m_visitor.get_rtt_time(p_context.m_address, &l_rtt, &l_remote_rtt);

			m_remote_host_rtt = l_rtt;
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