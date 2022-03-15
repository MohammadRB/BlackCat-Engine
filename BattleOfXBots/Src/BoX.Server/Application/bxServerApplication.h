// [06/15/2021 MRB]

#pragma once

#include "Core/Container/bcUnorderedMap.h"
#include "Core/Container/bcVector.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Messaging/Event/bcEventListenerHandle.h"
#include "Core/Utility/bcNullable.h"
#include "PlatformImp/Script/bcScriptContext.hpp"
#include "PlatformImp/Script/bcScriptObject.h"
#include "Game/System/Script/bcDefaultGameConsole.h"
#include "Game/System/Network/Server/bcNetworkServerManagerHook.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "App/Application/bcRenderApplication.h"
#include "BoX.Game/Application/bxDefinitions.h"
#include "BoX.Game/Application/bxGameState.h"
#include "BoX.Game/Application/bxServerNetworkMessageVisitor.h"

namespace box
{
	using namespace black_cat;

	enum class bx_app_state
	{
		initial,
		server_started,
		scene_loaded,
		game_started
	};

	struct bx_client
	{
		platform::bc_network_address m_address;
		game::bc_network_client_id m_id;
		core::bc_string m_name;

		core::bc_vector3f m_seat;
		core::bc_nullable<bx_team> m_team;
		game::bc_actor m_player_actor;
		bool m_is_dead = false;
		bcFLOAT m_dead_passed_time = 0;
	};

	struct bx_player_seat
	{
		game::bc_network_client_id m_client_id;
		core::bc_vector3f m_position;
	};

	class bx_server_application : public bc_render_application, game::bci_network_server_manager_hook, bx_server_network_message_visitor
	{
	public:
		static void bind(platform::bc_script_context& p_context, platform::bc_script_global_prototype_builder& p_global_prototype, bx_server_application& p_instance);
		
	private:
		// Application methods

		void application_start_engine_components(const bc_application_start_context& p_context) override;
		
		void application_initialize(const bc_application_initialize_context& p_context) override;
		
		void application_load_content(const bc_application_load_context& p_context) override;
		
		void application_update(const bc_application_update_context& p_context) override;
		
		void application_render(const bc_application_render_context& p_context) override;

		void application_swap_frame(const platform::bc_clock::update_param& p_clock) override;
		
		void application_event(core::bci_event& p_event) override;
		
		void application_unload_content(const bc_application_load_context& p_context) override;
		
		void application_destroy() override;
		
		void application_close_engine_components() override;

		// Network manager methods

		void started_listening(bcUINT16 p_port) noexcept override;

		void scene_changed(game::bc_scene* p_scene) noexcept override;

		core::bc_string client_connected(const game::bc_network_client& p_client) noexcept override;
		
		void client_disconnected(const game::bc_network_client& p_client) noexcept override;
		
		void message_packet_sent(const game::bc_network_client& p_client, 
			const core::bc_memory_stream& p_packet, 
			bcSIZE p_packet_size, 
			core::bc_const_span<game::bc_network_message_ptr> p_messages) noexcept override;

		void message_packet_received(const game::bc_network_client& p_client, 
			const core::bc_memory_stream& p_packet, 
			bcSIZE p_packet_size, 
			core::bc_const_span<game::bc_network_message_ptr> p_messages) noexcept override;
		
		void error_occurred(const game::bc_network_client* p_client, const bc_network_exception* p_exception) noexcept override;

		// Network visitor methods

		core::bc_string change_player_team(game::bc_network_client_id p_client_id, bx_team p_team) override;

		// Private methods

		void _send_game_state_to_clients(const platform::bc_clock::update_param& p_clock);

		void _create_scene_checkpoint(game::bc_scene& p_scene);

		void _restore_scene_checkpoint(game::bc_scene& p_scene);

		void _remove_client(game::bc_network_client_id p_id);

		void _reset_game(game::bc_scene& p_scene);

		std::pair<bool, core::bc_vector3f> _assign_seat(game::bc_network_client_id p_client_id, bx_team p_team);

		void _respawn_dead_players(const platform::bc_clock::update_param& p_clock);

		const bcECHAR* m_app_name{ nullptr };
		core::bc_unique_ptr<game::bc_default_game_console> m_console;
		platform::bc_script_context* m_server_script_context{ nullptr };
		platform::bc_script_object_ref m_server_script_object;
		game::bc_network_system* m_network_system{ nullptr };

		bx_app_state m_state = bx_app_state::initial;
		const bcFLOAT m_game_time = 60;
		const bcFLOAT m_respawn_time = 5;
		bcFLOAT m_current_game_time = m_game_time;
		bcFLOAT m_last_state_update_elapsed_ms = 0;
		std::unordered_map<game::bc_network_client_id, bx_client> m_joined_clients;
		core::bc_vector<bx_player_seat> m_red_player_seats;
		core::bc_vector<bx_player_seat> m_blue_player_seats;
		game::bc_scene* m_scene{ nullptr };

		core::bc_vector<core::bc_string> m_info_messages;
		core::bc_vector<bx_player_kill_state> m_killing_list;

		core::bc_event_listener_handle m_player_spawn_event_handle;
		core::bc_event_listener_handle m_player_kill_event_handle;
		core::bc_event_listener_handle m_player_remove_event_handle;
	};
}