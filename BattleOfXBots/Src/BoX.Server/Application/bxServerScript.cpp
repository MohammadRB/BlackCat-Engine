// [06/20/2021 MRB]

#include "Game/System/bcGameSystem.h"
#include "Game/System/Network/Message/bcStringNetworkMessage.h"
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

		m_game_system->get_network_system().start_server(*m_network_server_hook, p_port.as_integer());
		
		return platform::bc_script_variable();
	}

	platform::bc_script_variable bx_server_script::send(const platform::bc_script_variable& p_str_content)
	{
		if(!p_str_content.is_string())
		{
			return platform::bc_script_variable();
		}

		auto l_str = core::bc_to_string(p_str_content.as_string().data());
		m_game_system->get_network_system().send_message(game::bc_string_network_message(std::move(l_str)));
		
		return platform::bc_script_variable();
	}
}