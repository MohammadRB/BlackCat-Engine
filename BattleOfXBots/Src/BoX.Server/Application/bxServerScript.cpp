// [06/20/2021 MRB]

#include "Game/System/bcGameSystem.h"
#include "Game/System/Network/Message/bcStringNetworkMessage.h"
#include "Game/System/Network/Message/bcSceneChangeNetworkMessage.h"
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

	platform::bc_script_variable bx_server_script::load_scene(const platform::bc_script_variable& p_scene_name)
	{
		if (!p_scene_name.is_string())
		{
			return platform::bc_script_variable();
		}

		const core::bc_string_frame l_scene_extension(".json");
		auto l_scene_name = core::bc_to_string(p_scene_name.as_string().data());
		const bool l_end_with_json = l_scene_name.length() >= l_scene_extension.length()
			? std::equal
			(
				std::rbegin(l_scene_extension),
				std::rend(l_scene_extension),
				(std::rbegin(l_scene_name)),
				[](bcCHAR ch1, bcCHAR ch2)
				{
					return std::tolower(ch1) == std::tolower(ch2);
				}
			)
			: false;
		
		if(!l_end_with_json)
		{
			l_scene_name += l_scene_extension;
		}
		
		m_game_system->get_network_system().send_message(game::bc_scene_change_network_message(std::move(l_scene_name)));

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