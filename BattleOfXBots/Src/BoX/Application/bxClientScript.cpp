// [26/06/2021 MRB]

#include "PlatformImp/Script/bcScriptString.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/Network/bcNetworkSystem.h"
#include "Game/System/bcGameSystem.h"
#include "BoX.Game/Network/bxGameMessageNetworkMessage.h"
#include "BoX/Application/bxClientScript.h"

namespace box
{
	bx_client_script::bx_client_script(game::bc_game_system& p_game_system, 
		game::bci_network_client_manager_hook& p_network_client_hook, 
		game::bci_network_message_visitor& p_message_visitor)
		: m_game_system(&p_game_system),
		m_network_client_hook(&p_network_client_hook),
		m_message_visitor(&p_message_visitor)
	{
	}

	bx_client_script::bx_client_script(const bx_client_script&) noexcept = default;

	bx_client_script::~bx_client_script() = default;

	bx_client_script& bx_client_script::operator=(const bx_client_script&) noexcept = default;

	platform::bc_script_variable bx_client_script::change_name(const platform::bc_script_variable& p_name)
	{
		if (!p_name.is_string())
		{
			return {};
		}

		auto& l_config = bc_get_global_config();
		auto l_name = core::bc_to_string(p_name.as_string().data());
		l_config.set_network_client_name(std::move(l_name));
		l_config.flush_changes();

		return {};
	}

	platform::bc_script_variable bx_client_script::connect(const platform::bc_script_variable& p_ip, const platform::bc_script_variable& p_port)
	{
		if(p_ip.is_string() && p_port.is_number())
		{
			const auto l_ip = core::bc_to_string_frame(p_ip.as_string().data());
			const auto l_address = platform::bc_network_address::from_ip_port(platform::bc_socket_address::inter_network, l_ip.c_str(), p_port.as_integer());
			
			m_game_system->get_network_system().start_client(*m_network_client_hook, *m_message_visitor, l_address);
		}
		
		return {};
	}

	platform::bc_script_variable bx_client_script::say(const platform::bc_script_variable& p_str_content)
	{
		if (!p_str_content.is_string())
		{
			return {};
		}

		auto l_str = core::bc_to_string(p_str_content.as_string().data());
		m_game_system->get_network_system().send_message(bx_game_message_network_message(std::move(l_str)));

		return {};
	}
}