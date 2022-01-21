// [06/26/2021 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptVariable.h"
#include "Game/System/Network/Client/bcNetworkClientManagerHook.h"

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

	class bx_client_script
	{
	public:
		bx_client_script(game::bc_game_system& p_game_system, 
			game::bci_network_client_manager_hook& p_network_client_hook,
			game::bci_network_message_visitor& p_message_visitor);

		bx_client_script(const bx_client_script&) noexcept;

		~bx_client_script();

		bx_client_script& operator=(const bx_client_script&) noexcept;

		platform::bc_script_variable connect(const platform::bc_script_variable& p_ip, const platform::bc_script_variable& p_port);

		platform::bc_script_variable send(const platform::bc_script_variable& p_str_content);

	private:
		game::bc_game_system* m_game_system;
		game::bci_network_client_manager_hook* m_network_client_hook;
		game::bci_network_message_visitor* m_message_visitor;
	};
}