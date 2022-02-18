// [02/12/2022 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "BoX.Game/Application/bxDefinitions.h"
#include "BoX.Game/bxExport.h"

namespace box
{
	using namespace black_cat;

	class BX_GAME_DLL bx_team_select_network_message : public game::bci_network_message
	{
		BC_NETWORK_MESSAGE(tm_slt)
		
	public:
		bx_team_select_network_message() noexcept;

		explicit bx_team_select_network_message(bx_team p_selected_team) noexcept;

		bx_team_select_network_message(bx_team_select_network_message&&) noexcept = default;

		~bx_team_select_network_message() override = default;

		bx_team_select_network_message& operator=(bx_team_select_network_message&&) noexcept = default;

	private:
		bool need_acknowledgment() const noexcept override;

		core::bc_string get_acknowledgment_data() const noexcept override;

		void execute(const game::bc_network_message_server_context& p_context) noexcept override;

		void acknowledge(const game::bc_network_message_client_acknowledge_context& p_context) noexcept override;

		void serialize_message(const game::bc_network_message_serialization_context& p_context) override;

		void deserialize_message(const game::bc_network_message_deserialization_context& p_context) override;

		bx_team m_selected_team;
		core::bc_string m_error_message;
	};
}
