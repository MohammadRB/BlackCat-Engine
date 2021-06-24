// [06/20/2021 MRB]

#include "Game/System/bcGameSystem.h"
#include "BoX.Server/Application/bcServerScript.h"

namespace box
{
	bc_server_script::bc_server_script(game::bc_game_system& p_game_system)
		: m_game_system(&p_game_system)
	{
	}

	bc_server_script::bc_server_script(const bc_server_script&) noexcept = default;

	bc_server_script::~bc_server_script() = default;

	bc_server_script& bc_server_script::operator=(const bc_server_script&) noexcept = default;

	platform::bc_script_variable bc_server_script::start(const platform::bc_script_variable& p_port)
	{
		return platform::bc_script_variable();
	}
}