// [06/26/2021 MRB]

#include "Game/System/bcGameSystem.h"
#include "Game/System/Network/Message/bcStringNetworkMessage.h"
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
			const auto l_address = platform::bc_network_address::from_ip_port(platform::bc_socket_address::inter_network, l_ip.c_str(), p_port.as_integer());
			
			m_game_system->get_network_system().start_client(*m_network_client_hook, l_address);
		}
		
		return platform::bc_script_variable();
	}

	platform::bc_script_variable bx_client_script::send(const platform::bc_script_variable& p_str_content)
	{
		if (!p_str_content.is_string())
		{
			return platform::bc_script_variable();
		}

		auto l_str = core::bc_to_string(p_str_content.as_string().data());
		m_game_system->get_network_system().send_message(game::bc_string_network_message(std::move(l_str)));

		return platform::bc_script_variable();
	}
}