// [06/20/2021 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptContext.h"

namespace black_cat
{
	namespace game
	{
		class bc_game_system;
	}
}

using namespace black_cat;

namespace box
{
	class bc_server_script
	{
	public:
		bc_server_script(game::bc_game_system& p_game_system);

		bc_server_script(const bc_server_script&) noexcept;

		~bc_server_script();

		bc_server_script& operator=(const bc_server_script&) noexcept;

		void start(const platform::bc_script_int& p_port);

	private:
		game::bc_game_system* m_game_system;
	};
}