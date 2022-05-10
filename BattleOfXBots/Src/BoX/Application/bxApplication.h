// [06/20/2021 MRB]

#pragma once

#include "Core/Concurrency/bcTask.h"
#include "Core/Messaging/Event/bcEventListenerHandle.h"
#include "Game/System/Script/bcDefaultGameConsole.h"
#include "Game/System/Network/Client/bcNetworkClientManagerHook.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "App/Application/bcRenderApplication.h"
#include "BoX.Game/Application/bxClientNetworkMessageVisitor.h"
#include "BoX.Game/Application/bxPlayerService.h"

namespace box
{
	using namespace black_cat;

	enum class bx_app_state
	{
		initial,
		client_started,
		scene_loaded,
		game_started,
		game_scores
	};

	class bx_application : public bc_render_application, game::bci_network_client_manager_hook, bx_client_network_message_visitor
	{
	public:
		static void bind(platform::bc_script_context& p_context, platform::bc_script_global_prototype_builder& p_global_prototype, bx_application& p_instance);
		
	private:
		// Application methods

		void application_start_engine_components(const bc_application_start_context& p_context) override;

		void application_initialize(const bc_application_initialize_context& p_context) override;

		void application_load_content(const bc_application_load_context& p_context) override;

		void application_update(const bc_application_update_context& p_context) override;

		void application_render(const bc_application_render_context& p_context) override;

		void application_event(core::bci_event& p_event) override;

		void application_unload_content(const bc_application_load_context& p_context) override;

		void application_destroy() override;

		void application_close_engine_components() override;

		// Network manager methods

		void connecting_to_server(const platform::bc_network_address& p_server_address) noexcept override;
		
		void connection_to_server_approved(const platform::bc_network_address& p_server_address, const game::bc_client_connect_result& p_result) noexcept override;

		void scene_changed(game::bc_scene* p_scene) noexcept override;

		void message_packet_sent(const core::bc_memory_stream& p_packet, bcSIZE p_packet_size, core::bc_const_span<game::bc_network_message_ptr> p_messages) noexcept override;
		
		void message_packet_received(const core::bc_memory_stream& p_packet, bcSIZE p_packet_size, core::bc_const_span<game::bc_network_message_ptr> p_messages) noexcept override;
		
		void error_occurred(const bc_network_exception* p_exception) noexcept override;

		// Network visitor methods

		void update_game_state(const bx_game_state& p_state) override;

		void team_change_rejected(core::bc_string p_error) override;

		void spawn_player(const core::bc_vector3f& p_position, bx_team p_team) override;

		void show_scores() override;

		void reset_game() override;

		void message_received(core::bc_string p_msg) override;

		// Private methods

		void _create_scene_checkpoint(game::bc_scene& p_scene);

		void _restore_scene_checkpoint(game::bc_scene& p_scene, const platform::bc_clock::update_param& p_clock);

		void _reset_game(game::bc_scene& p_scene);

		void _reset_game_and_restore_scene_checkpoint(game::bc_scene& p_scene, const platform::bc_clock::update_param& p_clock);

		core::bc_unique_ptr<game::bc_default_game_console> m_console;
		platform::bc_script_context* m_client_script_context = nullptr;
		platform::bc_script_object_ref m_client_script_object;
		bx_player_service* m_player_service = nullptr;
		game::bc_scene* m_scene{ nullptr };

		bx_app_state m_state = bx_app_state::initial;
		core::bc_task<bx_team> m_team_select_task;
		core::bc_vector3f m_spawn_position{};
		bx_team m_team{};
		game::bc_actor m_player_actor;
		bool m_is_dead = false;
		bcFLOAT m_dead_passed_time = 0;
		bool m_reset_game_and_restore_checkpoint = { false };

		core::bc_event_listener_handle m_player_spawn_event_handle;
		core::bc_event_listener_handle m_player_kill_event_handle;
		core::bc_event_listener_handle m_player_remove_event_handle;
	};
}