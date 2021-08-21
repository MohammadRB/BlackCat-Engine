// [06/15/2021 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptContext.hpp"
#include "PlatformImp/Script/bcScriptObject.h"
#include "Game/System/Script/bcDefaultGameConsole.h"
#include "Game/System/Network/Server/bcNetworkServerManagerHook.h"
#include "BlackCat/Application/bcRenderApplication.h"

using namespace black_cat;

namespace box
{
	class bx_server_script;

	class bx_server_application : public bc_render_application, game::bci_network_server_manager_hook
	{
	public:
		static void bind(platform::bc_script_context& p_context, platform::bc_script_global_prototype_builder& p_global_prototype, bx_server_application& p_instance);
		
	private:
		void application_start_engine_components(game::bc_engine_application_parameter& p_parameters) override;
		
		void application_initialize(game::bc_engine_application_parameter& p_parameters) override;
		
		void application_load_content(core::bc_content_stream_manager& p_stream_manager) override;
		
		void application_update(core_platform::bc_clock::update_param p_clock, bool p_is_partial_update) override;
		
		void application_render(core_platform::bc_clock::update_param p_clock) override;
		
		bool application_event(core::bci_event& p_event) override;
		
		void application_unload_content(core::bc_content_stream_manager& p_stream_manager) override;
		
		void application_destroy() override;
		
		void application_close_engine_components() override;

		void started_listening(bcUINT16 p_port) override;
		
		void client_connected() override;
		
		void client_disconnected() override;
		
		void message_packet_sent(bcSIZE p_packet_size, core::bc_const_span<game::bc_network_message_ptr> p_messages) override;

		void message_packet_received(bcSIZE p_packet_size, core::bc_const_span<game::bc_network_message_ptr> p_messages) override;
		
		void error_occurred(const bc_network_exception* p_exception, const platform::bc_network_address* p_client) override;
	
		core::bc_unique_ptr<game::bc_default_game_console> m_console;
		platform::bc_script_context* m_server_script_context = nullptr;
		platform::bc_script_object_ref m_server_script_object;

		bool m_server_started = false;
	};
}