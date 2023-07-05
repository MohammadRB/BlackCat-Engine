// [22/10/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptVariable.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class bc_game_console;
		
	class BC_GAME_DLL bc_game_console_script
	{
	public:
		explicit bc_game_console_script(bc_game_console& p_game_console);

		bc_game_console_script(const bc_game_console_script&);

		~bc_game_console_script();

		bc_game_console_script& operator=(const bc_game_console_script&);

		platform::bc_script_variable disable_output(const platform::bc_script_variable& p_output);

		platform::bc_script_variable enable_output(const platform::bc_script_variable& p_output);

		platform::bc_script_variable clear_output();
			
	private:
		bc_game_console* m_game_console;
	};
}
