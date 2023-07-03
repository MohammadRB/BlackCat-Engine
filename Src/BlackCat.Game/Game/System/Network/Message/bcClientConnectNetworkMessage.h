// [02/07/2021 MRB]

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

			explicit bc_client_connect_network_message(core::bc_string p_client_name);

			bc_client_connect_network_message(bc_client_connect_network_message&&) noexcept;

			~bc_client_connect_network_message() override;

			bc_client_connect_network_message& operator=(bc_client_connect_network_message&&) noexcept;

			bool is_in_game_message() const noexcept override;

			bool need_acknowledgment() const noexcept override;

			core::bc_string get_acknowledgment_data() const noexcept override;
			
			void acknowledge(const bc_network_message_client_acknowledge_context& p_context) noexcept override;

			void execute(const bc_network_message_server_context& p_context) noexcept override;
			
		private:
			void serialize_message(const bc_network_message_serialization_context& p_context) override;
			
			void deserialize_message(const bc_network_message_deserialization_context& p_context) override;

			core::bc_string m_client_name;
			bc_client_connect_result m_result;
		};

		class BC_GAME_DLL bc_client_disconnect_network_message : public bci_network_message
		{
			BC_NETWORK_MESSAGE(cln_dct)

		public:
			bc_client_disconnect_network_message();

			bc_client_disconnect_network_message(bc_client_disconnect_network_message&&) noexcept;

			~bc_client_disconnect_network_message() override;

			bc_client_disconnect_network_message& operator=(bc_client_disconnect_network_message&&) noexcept;

			bool is_in_game_message() const noexcept override;

			void execute(const bc_network_message_server_context& p_context) noexcept override;
		
		private:
			void serialize_message(const bc_network_message_serialization_context& p_context) override;
			
			void deserialize_message(const bc_network_message_deserialization_context& p_context) override;
		};

		inline bool bc_client_connect_network_message::is_in_game_message() const noexcept
		{
			return false;
		}

		inline bool bc_client_connect_network_message::need_acknowledgment() const noexcept
		{
			return true;
		}

		inline bool bc_client_disconnect_network_message::is_in_game_message() const noexcept
		{
			return false;
		}
	}
}