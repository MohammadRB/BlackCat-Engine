// [02/23/2022 MRB]

#pragma once

#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "BoX.Game/bxExport.h"

namespace box
{
	using namespace black_cat;

	class BX_GAME_DLL bx_game_reset_network_message : public game::bci_network_message
	{
		BC_NETWORK_MESSAGE(gme_rst)

	public:
		bx_game_reset_network_message() noexcept;

		bx_game_reset_network_message(bx_game_reset_network_message&&) noexcept = default;

		~bx_game_reset_network_message() override = default;

		bx_game_reset_network_message& operator=(bx_game_reset_network_message&&) noexcept = default;

		bool need_acknowledgment() const noexcept override;

	private:
		void serialize_message(const game::bc_network_message_serialization_context& p_context) override;

		void deserialize_message(const game::bc_network_message_deserialization_context& p_context) override;

		void execute(const game::bc_network_message_client_context& p_context) noexcept override;
	};
}
