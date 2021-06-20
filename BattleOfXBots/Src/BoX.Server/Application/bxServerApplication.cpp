// [06/15/2021 MRB]

#include "Platform/bcEvent.h"
#include "BoX.Server/Application/bxServerApplication.h"

using namespace black_cat;

namespace box
{
	void bx_server_application::application_start_engine_components(game::bc_engine_application_parameter& p_parameters)
	{
		m_console = core::bc_make_unique<game::bc_default_game_console>(*this, m_game_system->get_console());
		m_console->show();
		m_console->get_console_window()->disable_close(false);
	}

	void bx_server_application::application_initialize(game::bc_engine_application_parameter& p_parameters)
	{
	}

	void bx_server_application::application_load_content(core::bc_content_stream_manager* p_stream_manager)
	{
	}

	void bx_server_application::application_update(core_platform::bc_clock::update_param p_clock, bool p_is_partial_update)
	{
	}

	void bx_server_application::application_render(core_platform::bc_clock::update_param p_clock)
	{
	}

	bool bx_server_application::application_event(core::bci_event& p_event)
	{
		auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();
		
		auto* l_close_event = core::bci_message::as<platform::bc_app_event_window_close>(p_event);
		if (l_close_event)
		{
			if (m_console->get_console_window()->get_id() == l_close_event->get_window_id())
			{
				platform::bc_app_event_exit l_exit_event(0);
				l_event_manager->process_event(l_exit_event);
			}

			return true;
		}
		
		return false;
	}

	void bx_server_application::application_unload_content(core::bc_content_stream_manager* p_stream_manager)
	{
	}

	void bx_server_application::application_destroy()
	{
	}

	void bx_server_application::application_close_engine_components()
	{
	}
}