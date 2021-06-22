// [10/22/2016 MRB]

#include "Game/GamePCH.h"

#include "PlatformImp/Script/bcScriptGlobalPrototypeBuilder.h"
#include "Game/System/Script/bcGameConsole.h"
#include "Game/System/Script/bcGameConsoleScript.h"

namespace black_cat
{
	namespace game
	{
		bc_game_console_script::bc_game_console_script(bc_game_console& p_game_console)
			: m_game_console(&p_game_console)
		{
		}

		bc_game_console_script::bc_game_console_script(const bc_game_console_script& p_other)
			: m_game_console(p_other.m_game_console)
		{
		}

		bc_game_console_script::~bc_game_console_script() = default;

		bc_game_console_script& bc_game_console_script::operator=(const bc_game_console_script& p_other)
		{
			m_game_console = p_other.m_game_console;
			
			return *this;
		}

		void bc_game_console_script::disable_output(const platform::bc_script_int& p_output)
		{
			m_game_console->disable_output(static_cast<bc_console_output_type>(p_output));
		}

		void bc_game_console_script::enable_output(const platform::bc_script_int& p_output)
		{
			m_game_console->enable_output(static_cast<bc_console_output_type>(p_output));
		}

		void bc_game_console_script::clear_output()
		{
			m_game_console->clear_output();
		}
	}	
}