// [25/02/2022 MRB]

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

	struct bx_player_score
	{
		core::bc_string m_name;
		bcUINT32 m_kill_count;
		bcUINT32 m_death_count;
	};

	struct bx_game_score
	{
		core::bc_vector<bx_player_score> m_red_team;
		core::bc_vector<bx_player_score> m_blue_team;
	};

	struct bx_game_state
	{
		bcUINT32 m_game_time;
		bx_game_score m_scores;
		core::bc_vector<core::bc_string> m_info_messages;
		core::bc_vector<bx_player_kill_state> m_killing_list;
	};
}
