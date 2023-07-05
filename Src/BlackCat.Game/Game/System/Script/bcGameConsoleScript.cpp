// [22/10/2016 MRB]

#include "Game/GamePCH.h"

#include "PlatformImp/Script/bcScriptGlobalPrototypeBuilder.h"
#include "Game/System/Script/bcGameConsole.h"
#include "Game/System/Script/bcGameConsoleScript.h"

namespace black_cat::game
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

	platform::bc_script_variable bc_game_console_script::disable_output(const platform::bc_script_variable& p_output)
	{
		m_game_console->disable_output(static_cast<bc_console_output_type>(p_output.as_integer()));
		return platform::bc_script_variable();
	}

	platform::bc_script_variable bc_game_console_script::enable_output(const platform::bc_script_variable& p_output)
	{
		m_game_console->enable_output(static_cast<bc_console_output_type>(p_output.as_integer()));
		return platform::bc_script_variable();
	}

	platform::bc_script_variable bc_game_console_script::clear_output()
	{
		m_game_console->clear_output();
		return platform::bc_script_variable();
	}
}
