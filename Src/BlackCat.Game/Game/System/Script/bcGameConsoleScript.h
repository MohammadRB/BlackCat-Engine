// [10/22/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptContext.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_game_console;
		
		class BC_GAME_DLL bc_game_console_script
		{
		public:
			explicit bc_game_console_script(bc_game_console& p_game_console);

			bc_game_console_script(const bc_game_console_script&);

			~bc_game_console_script();

			bc_game_console_script& operator=(const bc_game_console_script&);

			void disable_output(const platform::bc_script_int& p_output);

			void enable_output(const platform::bc_script_int& p_output);

			void clear_output();
			
		private:
			bc_game_console* m_game_console;
		};
	}	
}