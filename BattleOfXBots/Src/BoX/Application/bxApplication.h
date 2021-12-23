// [06/20/2021 MRB]

#pragma once

#include "Game/System/Script/bcDefaultGameConsole.h"
#include "Game/System/Network/Client/bcNetworkClientManagerHook.h"
#include "App/Application/bcRenderApplication.h"

namespace box
{
	using namespace black_cat;

	class bx_application : public bc_render_application, public game::bci_network_client_manager_hook
	{
	public:
		static void bind(platform::bc_script_context& p_context, platform::bc_script_global_prototype_builder& p_global_prototype, bx_application& p_instance);
		
	private:
		void application_start_engine_components(game::bc_engine_application_parameter& p_parameters) override;

		void application_initialize(game::bc_engine_application_parameter& p_parameters) override;

		void application_load_content(core::bc_content_stream_manager& p_stream_manager) override;

		void application_update(const core_platform::bc_clock::update_param& p_clock, bool p_is_partial_update) override;

		void application_render(const core_platform::bc_clock::update_param& p_clock) override;

		bool application_event(core::bci_event& p_event) override;

		void application_unload_content(core::bc_content_stream_manager& p_stream_manager) override;

		void application_destroy() override;

		void application_close_engine_components() override;

		void connecting_to_server(const platform::bc_network_address& p_address) override;
		
		void connected_to_server(const platform::bc_network_address& p_address) override;
		
		void message_packet_sent(const core::bc_memory_stream& p_packet, bcSIZE p_packet_size, core::bc_const_span<game::bc_network_message_ptr> p_messages) override;
		
		void message_packet_received(const core::bc_memory_stream& p_packet, bcSIZE p_packet_size, core::bc_const_span<game::bc_network_message_ptr> p_messages) override;
		
		void error_occurred(const bc_network_exception* p_exception) override;
	
		core::bc_unique_ptr<game::bc_default_game_console> m_console;
		platform::bc_script_context* m_client_script_context = nullptr;
		platform::bc_script_object_ref m_client_script_object;

		core_platform::bc_clock::small_delta_time m_client_start_time = 0;
	};
}