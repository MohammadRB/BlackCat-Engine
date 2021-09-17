// [06/15/2021 MRB]

#include "Core/Utility/bcLogger.h"
#include "Platform/bcEvent.h"
#include "PlatformImp/Script/bcScriptGlobalPrototypeBuilder.h"
#include "PlatformImp/Script/bcScriptPrototypeBuilder.h"
#include "Game/System/Script/bcScriptBinding.h"
#include "BlackCat/Loader/bcHeightMapLoaderDx11.h"
#include "BoX.Server/Application/bxServerApplication.h"
#include "BoX.Server/Application/bxServerScript.h"

using namespace black_cat;

namespace box
{
	void bx_server_application::bind(platform::bc_script_context& p_context, platform::bc_script_global_prototype_builder& p_global_prototype, bx_server_application& p_instance)
	{
		{
			platform::bc_script_context::scope l_scope(p_context);

			auto l_server_prototype_builder = p_context.create_prototype_builder<bx_server_script>();
			l_server_prototype_builder
				.function(L"start", &bx_server_script::start)
				.function(L"load_scene", &bx_server_script::load_scene)
				.function(L"send", &bx_server_script::send);

			auto l_server_prototype = p_context.create_prototype(l_server_prototype_builder);
			const auto l_server_object = p_context.create_object(l_server_prototype, bx_server_script(*p_instance.m_game_system, p_instance));
			
			p_instance.m_server_script_context = &p_context;
			p_instance.m_server_script_object.reset(l_server_object);

			platform::bc_script_property_descriptor<platform::bc_script_object> l_server_property(&p_instance.m_server_script_object.get(), false);
			p_global_prototype.property(L"server", l_server_property);
		}
	}
	
	void bx_server_application::application_start_engine_components(game::bc_engine_application_parameter& p_parameters)
	{
		auto& l_game_console = m_game_system->get_console();

		m_console = core::bc_make_unique<game::bc_default_game_console>(*this, l_game_console);
		m_console->show();
		m_console->get_console_window()->disable_close(false);
		m_console->get_console_window()->set_caption(p_parameters.m_app_parameters.m_app_name);
		l_game_console.set_implementation(m_console.get());

		core::bc_register_loader<game::bc_height_map, bc_height_map_loader_dx11>
		(
			"height_map", core::bc_make_loader<bc_height_map_loader_dx11>()
		);
	}

	void bx_server_application::application_initialize(game::bc_engine_application_parameter& p_parameters)
	{
		auto& l_script_system = m_game_system->get_script_system();
		auto l_script_binder = l_script_system.get_script_binder();
		l_script_binder.bind(game::bc_script_context::app, *this);
	}

	void bx_server_application::application_load_content(core::bc_content_stream_manager& p_stream_manager)
	{
	}

	void bx_server_application::application_update(core_platform::bc_clock::update_param p_clock, bool p_is_partial_update)
	{
#ifdef BC_DEBUG
		if(!m_server_started)
		{
			auto& l_script_system = m_game_system->get_script_system();
			l_script_system.run_script_throw(game::bc_script_context::app, L"server.start(6699);");
			l_script_system.run_script_throw(game::bc_script_context::app, L"server.load_scene(\"test\");");

			m_server_started = true;
		}
#endif
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

	void bx_server_application::application_unload_content(core::bc_content_stream_manager& p_stream_manager)
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
		m_game_system->get_console().set_implementation(nullptr);
		m_console.reset();
	}

	void bx_server_application::application_close_engine_components()
	{
	}

	void bx_server_application::started_listening(bcUINT16 p_port)
	{
		//core::bc_log(core::bc_log_type::info) << "started listening on port " << p_port << core::bc_lend;
	}

	void bx_server_application::client_connected()
	{
		core::bc_log(core::bc_log_type::info) << "new client connected" << core::bc_lend;
	}

	void bx_server_application::client_disconnected()
	{
		core::bc_log(core::bc_log_type::info) << "client disconnected" << core::bc_lend;
	}

	void bx_server_application::message_packet_sent(bcSIZE p_packet_size, core::bc_const_span<game::bc_network_message_ptr> p_messages)
	{
		//core::bc_log(core::bc_log_type::debug) << "network message packet sent with size of '" << p_packet_size << "' bytes" << core::bc_lend;
		//core::bc_log(core::bc_log_type::debug) << "network message sent with hash '" << p_message.get_message_name() << "' and id " << p_message.get_id() << core::bc_lend;
	}

	void bx_server_application::message_packet_received(bcSIZE p_packet_size, core::bc_const_span<game::bc_network_message_ptr> p_messages)
	{
		//core::bc_log(core::bc_log_type::debug) << "network message packet received with size of '" << p_packet_size << "' bytes" << core::bc_lend;
		//core::bc_log(core::bc_log_type::debug) << "network message received with hash '" << p_message.get_message_name() << "' and id " << p_message.get_id() << core::bc_lend;
	}

	void bx_server_application::error_occurred(const bc_network_exception* p_exception, const platform::bc_network_address* p_client)
	{
		if(p_client)
		{
			core::bc_log(core::bc_log_type::error) << "error occurred in client network connection (" << std::get<core::bc_string_frame>(p_client->get_traits()) << "): ";
		}
		else
		{
			core::bc_log(core::bc_log_type::error) << "error occurred in network connection: ";
		}
		
		core::bc_log(core::bc_log_type::error) <<  (p_exception ? p_exception->get_full_message().c_str() : "") << core::bc_lend;
	}
}