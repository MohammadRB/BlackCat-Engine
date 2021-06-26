// [06/26/2021 MRB]

#include "Game/System/bcGameSystem.h"
#include "BoX/Application/bxClientScript.h"

namespace box
{
	bx_client_script::bx_client_script(game::bc_game_system& p_game_system, game::bci_network_client_manager_hook& p_network_client_hook)
		: m_game_system(&p_game_system),
		m_network_client_hook(&p_network_client_hook)
	{
	}

	bx_client_script::bx_client_script(const bx_client_script&) noexcept = default;

	bx_client_script::~bx_client_script() = default;

	bx_client_script& bx_client_script::operator=(const bx_client_script&) noexcept = default;

	platform::bc_script_variable bx_client_script::connect(const platform::bc_script_variable& p_ip, const platform::bc_script_variable& p_port)
	{
		if(p_ip.is_string() && p_port.is_number())
		{
			const auto l_ip = core::bc_to_string_frame(p_ip.as_string().data());
			m_game_system->get_network_system().start_client(l_ip.c_str(), p_port.as_integer(), *m_network_client_hook);
		}
		
		return platform::bc_script_variable();
	}
}