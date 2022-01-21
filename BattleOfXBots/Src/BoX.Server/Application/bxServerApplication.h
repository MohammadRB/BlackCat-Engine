// [06/15/2021 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptContext.hpp"
#include "PlatformImp/Script/bcScriptObject.h"
#include "Game/System/Script/bcDefaultGameConsole.h"
#include "Game/System/Network/Server/bcNetworkServerManagerHook.h"
#include "App/Application/bcRenderApplication.h"

namespace box
{
	using namespace black_cat;
	class bx_server_script;

	class bx_server_application : public bc_render_application, game::bci_network_server_manager_hook, game::bci_network_message_visitor
	{
	public:
		static void bind(platform::bc_script_context& p_context, platform::bc_script_global_prototype_builder& p_global_prototype, bx_server_application& p_instance);
		
	private:
		void application_start_engine_components(game::bc_engine_application_parameter& p_parameters) override;
		
		void application_initialize(game::bc_engine_application_parameter& p_parameters) override;
		
		void application_load_content(core::bc_content_stream_manager& p_stream_manager) override;
		
		void application_update(const core_platform::bc_clock::update_param& p_clock, bool p_is_partial_update) override;
		
		void application_render(const core_platform::bc_clock::update_param& p_clock) override;

		void application_swap_frame(const core_platform::bc_clock::update_param& p_clock) override;
		
		bool application_event(core::bci_event& p_event) override;
		
		void application_unload_content(core::bc_content_stream_manager& p_stream_manager) override;
		
		void application_destroy() override;
		
		void application_close_engine_components() override;

		void started_listening(bcUINT16 p_port) noexcept override;
		
		core::bc_string client_connected(const game::bc_network_client& p_client) noexcept override;
		
		void client_disconnected(const game::bc_network_client& p_client) noexcept override;
		
		void message_packet_sent(const game::bc_network_client& p_client, const core::bc_memory_stream& p_packet, bcSIZE p_packet_size, core::bc_const_span<game::bc_network_message_ptr> p_messages) noexcept override;

		void message_packet_received(const game::bc_network_client& p_client, const core::bc_memory_stream& p_packet, bcSIZE p_packet_size, core::bc_const_span<game::bc_network_message_ptr> p_messages) noexcept override;
		
		void error_occurred(const game::bc_network_client* p_client, const bc_network_exception* p_exception) noexcept override;

		const bcECHAR* m_app_name{ nullptr };
		core::bc_unique_ptr<game::bc_default_game_console> m_console;
		platform::bc_script_context* m_server_script_context = nullptr;
		platform::bc_script_object_ref m_server_script_object;

		bool m_server_started = false;
	};
}