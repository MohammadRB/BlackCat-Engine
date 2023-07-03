// [20/06/2021 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptVariable.h"
#include "Game/System/Network/Server/bcNetworkServerManagerHook.h"

namespace black_cat
{
	namespace game
	{
		class bc_game_system;
	}
}

namespace box
{
	using namespace black_cat;

	class bx_server_script
	{
	public:
		bx_server_script(game::bc_game_system& p_game_system, 
			game::bci_network_server_manager_hook& p_network_server_hook, 
			game::bci_network_message_visitor& p_message_visitor);

		bx_server_script(const bx_server_script&) noexcept;

		~bx_server_script();

		bx_server_script& operator=(const bx_server_script&) noexcept;

		platform::bc_script_variable start(const platform::bc_script_variable& p_port);

		platform::bc_script_variable load_scene(const platform::bc_script_variable& p_scene_name);
		
		platform::bc_script_variable say(const platform::bc_script_variable& p_str_content);

	private:
		game::bc_game_system* m_game_system;
		game::bci_network_server_manager_hook* m_network_server_hook;
		game::bci_network_message_visitor* m_message_visitor;
	};
}