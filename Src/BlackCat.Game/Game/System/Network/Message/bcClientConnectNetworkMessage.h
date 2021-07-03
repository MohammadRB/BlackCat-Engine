// [07/02/2021 MRB]

#pragma once

#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_client_connect_network_message : public bci_network_message
		{
			BC_NETWORK_MESSAGE(cln_cnt)

		public:
			bc_client_connect_network_message();

			bc_client_connect_network_message(bc_client_connect_network_message&&) noexcept;

			~bc_client_connect_network_message() override;

			bc_client_connect_network_message& operator=(bc_client_connect_network_message&&) noexcept;

			bool need_acknowledgment() const noexcept override;
			
			void acknowledge(const bc_network_message_client_context& p_context) noexcept override;

			void execute(const bc_network_message_server_context& p_context) noexcept override;
			
		private:
			void serialize_message(core::bc_json_key_value& p_params) const override;
			
			void deserialize_message(const core::bc_json_key_value& p_params) override;
		};

		class bc_client_disconnect_network_message : bci_network_message
		{
			BC_NETWORK_MESSAGE(cln_dct)

		public:
			bc_client_disconnect_network_message();

			bc_client_disconnect_network_message(bc_client_disconnect_network_message&&) noexcept;

			~bc_client_disconnect_network_message() override;

			bc_client_disconnect_network_message& operator=(bc_client_disconnect_network_message&&) noexcept;

			void execute(const bc_network_message_server_context& p_context) noexcept override;
		
		private:
			void serialize_message(core::bc_json_key_value& p_params) const override;
			
			void deserialize_message(const core::bc_json_key_value& p_params) override;
		};

		inline bc_client_connect_network_message::bc_client_connect_network_message()
			: bci_network_message(message_name())
		{
		}

		inline bc_client_connect_network_message::bc_client_connect_network_message(bc_client_connect_network_message&&) noexcept = default;

		inline bc_client_connect_network_message::~bc_client_connect_network_message() = default;

		inline bc_client_connect_network_message& bc_client_connect_network_message::operator=(bc_client_connect_network_message&&) noexcept = default;

		inline bool bc_client_connect_network_message::need_acknowledgment() const noexcept
		{
			return true;
		}

		inline bc_client_disconnect_network_message::bc_client_disconnect_network_message()
			: bci_network_message(message_name())
		{
		}

		inline bc_client_disconnect_network_message::bc_client_disconnect_network_message(bc_client_disconnect_network_message&&) noexcept = default;

		inline bc_client_disconnect_network_message::~bc_client_disconnect_network_message() = default;

		inline bc_client_disconnect_network_message& bc_client_disconnect_network_message::operator=(bc_client_disconnect_network_message&&) noexcept = default;
	}	
}