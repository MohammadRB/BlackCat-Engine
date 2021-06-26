// [06/20/2021 MRB]

#include "Core/Utility/bcLogger.h"
#include "PlatformImp/Script/bcScriptGlobalPrototypeBuilder.h"
#include "PlatformImp/Script/bcScriptPrototypeBuilder.h"
#include "Game/System/Script/bcScriptBinding.h"
#include "Game/System/Script/bcScriptSystem.h"
#include "BoX/Application/bxApplication.h"
#include "BoX/Application/bxClientScript.h"

namespace box
{
	void bx_application::bind(platform::bc_script_context& p_context, platform::bc_script_global_prototype_builder& p_global_prototype, bx_application& p_instance)
	{
		{
			platform::bc_script_context::scope l_scope(p_context);

			auto l_client_prototype_builder = p_context.create_prototype_builder<bx_client_script>();
			l_client_prototype_builder.function(bcL("connect"), &bx_client_script::connect);

			auto l_client_prototype = p_context.create_prototype(l_client_prototype_builder);
			const auto l_client_object = p_context.create_object(l_client_prototype, bx_client_script(*p_instance.m_game_system, p_instance));
			
			p_instance.m_client_script_context = &p_context;
			p_instance.m_client_script_object.reset(l_client_object);

			platform::bc_script_property_descriptor<platform::bc_script_object> l_client_property(&p_instance.m_client_script_object.get(), false);
			p_global_prototype.property(L"client", l_client_property);
		}
	}

	void bx_application::application_start_engine_components(game::bc_engine_application_parameter& p_parameters)
	{
		auto& l_game_console = m_game_system->get_console();

		m_console = core::bc_make_unique<game::bc_default_game_console>(*this, l_game_console);
		m_console->show();
		m_console->get_console_window()->set_caption(p_parameters.m_app_parameters.m_app_name);
		l_game_console.set_implementation(m_console.get());
	}

	void bx_application::application_initialize(game::bc_engine_application_parameter& p_parameters)
	{
		auto& l_script_system = m_game_system->get_script_system();
		auto l_script_binder = l_script_system.get_script_binder();
		l_script_binder.bind(game::bc_script_context::app, *this);
	}

	void bx_application::application_load_content(core::bc_content_stream_manager* p_stream_manager)
	{
	}

	void bx_application::application_update(core_platform::bc_clock::update_param p_clock, bool p_is_partial_update)
	{
#ifdef BC_DEBUG
		constexpr auto l_client_start_delay = 1.0f;
		if(m_client_start_time <= l_client_start_delay && !p_is_partial_update)
		{
			m_client_start_time += p_clock.m_elapsed_second;
			if(m_client_start_time > l_client_start_delay)
			{
				m_game_system->get_network_system().start_client("127.0.0.1", 6699, *this);
			}
		}
#endif
	}

	void bx_application::application_render(core_platform::bc_clock::update_param p_clock)
	{
	}

	bool bx_application::application_event(core::bci_event& p_event)
	{
		return false;
	}

	void bx_application::application_unload_content(core::bc_content_stream_manager* p_stream_manager)
	{
	}

	void bx_application::application_destroy()
	{
		if (m_client_script_context->is_valid())
		{
			{
				platform::bc_script_context::scope l_scope(*m_client_script_context);
				m_client_script_object.reset();
			}
		}
		m_console.reset();
	}

	void bx_application::application_close_engine_components()
	{
	}

	void bx_application::connecting_to_server(const bcCHAR* p_ip, bcUINT16 p_port)
	{
		core::bc_log(core::bc_log_type::info) << "connecting to server " << p_ip << ":" << p_port << core::bc_lend;
	}

	void bx_application::connected_to_server(const bcCHAR* p_ip, bcUINT16 p_port)
	{
		core::bc_log(core::bc_log_type::info) << "connected to server" << core::bc_lend;
	}

	void bx_application::message_sent(game::bci_network_message& p_message)
	{
	}

	void bx_application::message_received(game::bci_network_message& p_message)
	{
	}

	void bx_application::error_occurred(const bc_network_exception* p_exception)
	{
		core::bc_log(core::bc_log_type::error) << "error occurred in network connection: " << (p_exception ? p_exception->get_full_message().c_str() : "") << core::bc_lend;
	}
}