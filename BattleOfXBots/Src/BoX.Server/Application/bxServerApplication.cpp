// [06/15/2021 MRB]

#include "Platform/bcEvent.h"
#include "PlatformImp/Script/bcScriptGlobalPrototypeBuilder.h"
#include "PlatformImp/Script/bcScriptPrototypeBuilder.h"
#include "Game/System/Script/bcScriptBinding.h"
#include "BoX.Server/Application/bxServerApplication.h"
#include "BoX.Server/Application/bcServerScript.h"

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
		auto& l_script_system = m_game_system->get_script_system();
		auto l_script_binder = l_script_system.get_script_binder();
		l_script_binder.bind(game::bc_script_context::app, *this);
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
		if (m_server_script_context->is_valid())
		{
			{
				platform::bc_script_context::scope l_scope(*m_server_script_context);
				m_server_script_object.reset();
			}
		}
	}

	void bx_server_application::application_close_engine_components()
	{
	}

	void bx_server_application::bind(platform::bc_script_context& p_context, platform::bc_script_global_prototype_builder& p_global_prototype, bx_server_application& p_instance)
	{
		{
			platform::bc_script_context::scope l_scope(p_context);

			auto l_server_prototype_builder = p_context.create_prototype_builder<bc_server_script>();
			l_server_prototype_builder.function(L"start", &bc_server_script::start);

			auto l_server_prototype = p_context.create_prototype(l_server_prototype_builder);
			p_instance.m_server_script_object.reset(p_context.create_object(l_server_prototype, bc_server_script(*p_instance.m_game_system)));

			platform::bc_script_property_descriptor<platform::bc_script_object> l_server_property(&p_instance.m_server_script_object.get(), false);
			p_global_prototype.property(L"server", l_server_property);
		}
	}
}