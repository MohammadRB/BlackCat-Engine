// [10/05/2021 MRB]

#pragma once

#include "Game/System/Network/bcNetworkDefinitions.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_ping_network_message : public bci_network_message
		{
			BC_NETWORK_MESSAGE(ping)
			
		public:
			bc_ping_network_message();

			bc_ping_network_message(bc_ping_network_message&&) noexcept;

			~bc_ping_network_message() override;

			bc_ping_network_message& operator=(bc_ping_network_message&&) noexcept;

			bool need_acknowledgment() const noexcept override;

			core::bc_string get_acknowledgment_data() const noexcept override;
			
			void acknowledge(const bc_network_message_client_acknowledge_context& p_context) noexcept override;
			
			void acknowledge(const bc_network_message_server_acknowledge_context& p_context) noexcept override;

			void execute(const bc_network_message_client_context& p_context) noexcept override;
			
			void execute(const bc_network_message_server_context& p_context) noexcept override;
		
		private:
			void serialize_message(const bc_network_message_serialization_context& p_context) override;

			void deserialize_message(const bc_network_message_deserialization_context& p_context) override;
			
			bc_network_packet_time m_serialize_time;
			bc_network_rtt m_remote_host_rtt;
		};
	}	
}