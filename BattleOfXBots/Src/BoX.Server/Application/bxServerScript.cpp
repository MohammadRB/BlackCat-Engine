// [06/20/2021 MRB]

#include "Game/System/bcGameSystem.h"
#include "BoX.Server/Application/bxServerScript.h"

namespace box
{
	bx_server_script::bx_server_script(game::bc_game_system& p_game_system, game::bci_network_server_manager_hook& p_network_server_hook)
		: m_game_system(&p_game_system),
		m_network_server_hook(&p_network_server_hook)
	{
	}

	bx_server_script::bx_server_script(const bx_server_script&) noexcept = default;

	bx_server_script::~bx_server_script() = default;

	bx_server_script& bx_server_script::operator=(const bx_server_script&) noexcept = default;

	platform::bc_script_variable bx_server_script::start(const platform::bc_script_variable& p_port)
	{
		if(!p_port.is_valid())
		{
			return platform::bc_script_variable();
		}

		m_game_system->get_network_system().start_server(p_port.as_integer(), *m_network_server_hook);
		
		return platform::bc_script_variable();
	}
}