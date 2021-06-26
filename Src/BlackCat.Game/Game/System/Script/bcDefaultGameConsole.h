 // [10/24/2016 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Container/bcString.h"
#include "Core/Concurrency/bcTask.h"
#include "Core/Utility/bcNullable.h"
#include "PlatformImp/Application/bcConsoleWindow.h"
#include "Game/bcExport.h"
#include "Game/Application/bcRenderApplication.h"
#include "Game/System/Script/bcIGameConsoleImp.h"
#include "Game/System/Script/bcGameConsole.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_default_game_console : public bci_game_console_imp
		{
		public:
			bc_default_game_console(bc_render_application& p_application, bc_game_console& p_game_console) noexcept;

			bc_default_game_console(bc_default_game_console&&) noexcept;

			~bc_default_game_console() override;

			bc_default_game_console& operator=(bc_default_game_console&&) noexcept;

			platform::bc_console_window* get_console_window() noexcept;
			
			const platform::bc_console_window* get_console_window() const noexcept;
			
			void write_output(bc_console_output_type p_type, const bcECHAR* p_msg) override;

			void clear_output() override;

			void show() override;

			void hide() override;

			bool is_visible() override;

			void update(const core_platform::bc_clock::update_param& p_clock_update_param) override;

		private:
			void _input_spin();

			bc_render_application* m_application;
			core_platform::bc_mutex m_console_mutex;
			core::bc_nullable<platform::bc_console_window> m_console;
			core::bc_task<void> m_input_spin_task;
			core_platform::bc_mutex m_input_mutex;
			core::bc_string m_input_line;
		};
	}
}