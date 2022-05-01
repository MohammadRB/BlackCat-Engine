// [02/11/2022 MRB]

#pragma once

#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "BoX.Game/Application/bxDefinitions.h"
#include "BoX.Game/Application/bxGameState.h"
#include "BoX.Game/bxExport.h"

namespace box
{
	using namespace black_cat;

	class BX_GAME_DLL bx_client_network_message_visitor : public game::bci_network_message_visitor
	{
	public:
		virtual void update_game_state(const bx_game_state& p_state) = 0;

		virtual void team_change_rejected(core::bc_string p_error) = 0;

		virtual void spawn_player(const core::bc_vector3f& p_position, bx_team p_team) = 0;

		virtual void show_scores() = 0;

		virtual void reset_game() = 0;

		virtual void message_received(core::bc_string p_msg) = 0;
	};
}
