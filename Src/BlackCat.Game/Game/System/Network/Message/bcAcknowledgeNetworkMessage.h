// [06/06/2021 MRB]

#pragma once

#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_acknowledge_network_message : public bci_network_message
		{
			BC_NETWORK_MESSAGE(ack)

		public:
			bc_acknowledge_network_message();

			explicit bc_acknowledge_network_message(const bci_network_message& p_message);

			bc_acknowledge_network_message(bc_acknowledge_network_message&&) noexcept;

			~bc_acknowledge_network_message() override;

			bc_acknowledge_network_message& operator=(bc_acknowledge_network_message&&) noexcept;

			bc_network_message_id get_acknowledged_message_id() const noexcept;

			core::bc_string get_acknowledged_message_data() const noexcept;
			
			void execute(const bc_network_message_client_context& p_context) noexcept override;
			
			void execute(const bc_network_message_server_context& p_context) noexcept override;
		
		private:
			void serialize_message(const bc_network_message_serialization_context& p_context) override;

			void deserialize_message(const bc_network_message_deserialization_context& p_context) override;

			bc_network_message_id m_ack_id;
			mutable core::bc_string m_ack_data;
		};

		inline bc_network_message_id bc_acknowledge_network_message::get_acknowledged_message_id() const noexcept
		{
			return m_ack_id;
		}

		inline core::bc_string bc_acknowledge_network_message::get_acknowledged_message_data() const noexcept
		{
			return std::move(m_ack_data);
		}
	}
}