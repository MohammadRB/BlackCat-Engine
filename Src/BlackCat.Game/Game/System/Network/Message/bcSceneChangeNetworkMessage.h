// [08/10/2021 MRB]

#pragma once

#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_scene_change_network_message : public bci_network_message
		{
			BC_NETWORK_MESSAGE(scn_chg)

		public:
			bc_scene_change_network_message();

			explicit bc_scene_change_network_message(core::bc_string p_scene_name);

			bc_scene_change_network_message(bc_scene_change_network_message&&) noexcept;

			~bc_scene_change_network_message() override;

			bc_scene_change_network_message& operator=(bc_scene_change_network_message&&) noexcept;

			bool is_in_game_message() const noexcept override;

			bool need_acknowledgment() const noexcept override;

			void execute(const bc_network_message_client_context& p_context) noexcept override;
			
		private:
			void serialize_message(const bc_network_message_serialization_context& p_context) override;

			void deserialize_message(const bc_network_message_deserialization_context& p_context) override;

			core::bc_string m_scene_name;
		};

		inline bool bc_scene_change_network_message::is_in_game_message() const noexcept
		{
			return false;
		}

		inline bool bc_scene_change_network_message::need_acknowledgment() const noexcept
		{
			return true;
		}
	}
}