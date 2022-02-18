// [02/11/2022 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "BoX.Game/bxExport.h"

namespace box
{
	using namespace black_cat;

	class BX_GAME_DLL bx_server_network_message_visitor : public game::bci_network_message_visitor
	{
	public:
		virtual core::bc_string change_player_team(game::bc_network_client_id p_client_id, bx_team p_team) = 0;
	};
}
