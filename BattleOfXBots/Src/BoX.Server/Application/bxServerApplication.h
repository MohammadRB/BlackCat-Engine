// [06/15/2021 MRB]

#pragma once

#include "Game/System/Script/bcDefaultGameConsole.h"
#include "BlackCat/Application/bcRenderApplication.h"

using namespace black_cat;

namespace box
{
	class bx_server_application : public bc_render_application
	{
	private:
		void application_start_engine_components(game::bc_engine_application_parameter& p_parameters) override;
		
		void application_initialize(game::bc_engine_application_parameter& p_parameters) override;
		
		void application_load_content(core::bc_content_stream_manager* p_stream_manager) override;
		
		void application_update(core_platform::bc_clock::update_param p_clock, bool p_is_partial_update) override;
		
		void application_render(core_platform::bc_clock::update_param p_clock) override;
		
		bool application_event(core::bci_event& p_event) override;
		
		void application_unload_content(core::bc_content_stream_manager* p_stream_manager) override;
		
		void application_destroy() override;
		
		void application_close_engine_components() override;
	
		core::bc_unique_ptr<game::bc_default_game_console> m_console;
	};
}