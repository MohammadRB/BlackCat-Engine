// [02/25/2022 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "BoX.Game/Application/bxDefinitions.h"

namespace box
{
	using namespace black_cat;

	struct bx_player_kill_state
	{
		bx_team m_killer_team;
		core::bc_string m_killer_name;
		bx_team m_killed_team;
		core::bc_string m_killed_name;
	};

	struct bx_game_state
	{
		bcUINT32 m_game_time;
		core::bc_vector<core::bc_string> m_info_messages;
		core::bc_vector<bx_player_kill_state> m_killing_list;
	};
}
