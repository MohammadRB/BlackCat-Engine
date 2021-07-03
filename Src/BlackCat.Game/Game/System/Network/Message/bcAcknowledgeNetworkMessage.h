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

			explicit bc_acknowledge_network_message(bc_network_message_id p_acknowledged_message_id);

			bc_acknowledge_network_message(bc_acknowledge_network_message&&) noexcept;

			~bc_acknowledge_network_message() override;

			bc_acknowledge_network_message& operator=(bc_acknowledge_network_message&&) noexcept;

			bc_network_message_id get_acknowledged_message_id() const noexcept;

			void execute(const bc_network_message_client_context& p_context) noexcept override;
			
			void execute(const bc_network_message_server_context& p_context) noexcept override;
		
		private:
			void serialize_message(core::bc_json_key_value& p_params) const override;

			void deserialize_message(const core::bc_json_key_value& p_params) override;

			bc_network_message_id m_ack_message_id;
		};

		inline bc_acknowledge_network_message::bc_acknowledge_network_message()
			: bci_network_message(message_name()),
			m_ack_message_id(0)
		{
		}

		inline bc_acknowledge_network_message::bc_acknowledge_network_message(bc_network_message_id p_acknowledged_message_id)
			: bc_acknowledge_network_message()
		{
			m_ack_message_id = p_acknowledged_message_id;
		}

		inline bc_acknowledge_network_message::bc_acknowledge_network_message(bc_acknowledge_network_message&&) noexcept = default;

		inline bc_acknowledge_network_message::~bc_acknowledge_network_message() = default;

		inline bc_acknowledge_network_message& bc_acknowledge_network_message::operator=(bc_acknowledge_network_message&&) noexcept = default;

		inline bc_network_message_id bc_acknowledge_network_message::get_acknowledged_message_id() const noexcept
		{
			return m_ack_message_id;
		}
	}
}