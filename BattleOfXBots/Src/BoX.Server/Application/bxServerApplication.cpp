// [06/15/2021 MRB]

#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Content/bcContentManager.h"
#include "Core/Utility/bcLogger.h"
#include "Platform/bcEvent.h"
#include "PlatformImp/Script/bcScriptGlobalPrototypeBuilder.h"
#include "PlatformImp/Script/bcScriptPrototypeBuilder.h"
#include "SoundImp/Resource/bcChannelGroup.h"
#include "Game/System/Sound/bcSoundSystem.h"
#include "Game/System/Script/bcScriptBinding.h"
#include "Game/Object/Scene/bcScene.h"
#include "App/Loader/bcSceneCheckPointLoader.h"
#include "BoX.Game/Application/bxApplicationHookFunctions.h"
#include "BoX.Game/Application/bxSceneCheckPoint.h"
#include "BoX.Game/Game/bxPlayerSeatComponent.h"
#include "BoX.Game/Network/bxPlayerSpawnNetworkMessage.h"
#include "BoX.Game/Network/bxGameStateNetworkMessage.h"
#include "BoX.Game/Network/bxGameResetNetworkMessage.h"
#include "BoX.Game/Network/bxGameMessageNetworkMessage.h"
#include "BoX.Game/Network/bxGameScoresNetworkMessage.h"
#include "BoX.Game/bxEvent.h"
#include "BoX.Server/Application/bxServerApplication.h"
#include "BoX.Server/Application/bxServerScript.h"

using namespace black_cat;

namespace box
{
	bool g_server_started = false;

	void bx_server_application::bind(platform::bc_script_context& p_context, platform::bc_script_global_prototype_builder& p_global_prototype, bx_server_application& p_instance)
	{
		{
			platform::bc_script_context::scope l_scope(p_context);

			auto l_server_prototype_builder = p_context.create_prototype_builder<bx_server_script>();
			l_server_prototype_builder
				.function(L"start", &bx_server_script::start)
				.function(L"load_scene", &bx_server_script::load_scene)
				.function(L"say", &bx_server_script::say);

			auto l_server_prototype = p_context.create_prototype(l_server_prototype_builder);
			const auto l_server_object = p_context.create_object(l_server_prototype, bx_server_script(*p_instance.m_game_system, p_instance, p_instance));
			
			p_instance.m_server_script_context = &p_context;
			p_instance.m_server_script_object.reset(l_server_object);

			platform::bc_script_property_descriptor<platform::bc_script_object> l_server_property(&p_instance.m_server_script_object.get(), false);
			p_global_prototype.property(L"server", l_server_property);
		}
	}
	
	void bx_server_application::application_start_engine_components(const bc_application_start_context& p_context)
	{
		bx_start_game_services(p_context);
		bx_register_game_loaders(p_context);
		bx_register_game_actor_components();
		bx_register_game_network_messages(m_game_system->get_network_system());
		bx_bind_game_scripts(*m_game_system);
		bx_register_game_particle_emitters(*m_game_system);
		bx_load_game_shaders(*core::bc_get_service<core::bc_content_stream_manager>(), *m_game_system);

		m_app_name = p_context.m_app_parameters.m_app_name;
		m_network_system = &m_game_system->get_network_system();
		m_player_service = core::bc_get_service<bx_player_list_service>();
	}

	void bx_server_application::application_initialize(const bc_application_initialize_context& p_context)
	{
		auto& l_event_manager = *core::bc_get_service<core::bc_event_manager>();
		auto& l_game_console = m_game_system->get_console();

		m_game_system->get_sound_system().get_device().get_default_channel_group().set_mute(true);

		m_console = core::bc_make_unique<game::bc_default_game_console>(*this, l_game_console);
		m_console->show();
		m_console->get_console_window()->disable_close(false);
		m_console->get_console_window()->set_caption(p_context.m_app_parameters.m_app_name);
		l_game_console.set_implementation(m_console.get());

#ifdef BC_DEBUG
		m_game_time = 60;
#else
		m_game_time = 600;
#endif

		m_player_spawn_event_handle = l_event_manager.register_event_listener<bx_player_spawned_event>
		(
			core::bc_event_manager::delegate_type(*this, &bx_server_application::application_event)
		);
		m_player_kill_event_handle = l_event_manager.register_event_listener<bx_player_killed_event>
		(
			core::bc_event_manager::delegate_type(*this, &bx_server_application::application_event)
		);
		m_player_remove_event_handle = l_event_manager.register_event_listener<bx_player_removed_event>
		(
			core::bc_event_manager::delegate_type(*this, &bx_server_application::application_event)
		);

		auto l_script_binder = m_game_system->get_script_system().get_script_binder();
		l_script_binder.bind(game::bc_script_context::app, *this);
	}

	void bx_server_application::application_load_content(const bc_application_load_context& p_context)
	{
		bx_load_game_resources(p_context.m_stream_manager, *m_game_system);
	}

	void bx_server_application::application_update(const bc_application_update_context& p_context)
	{
		if(p_context.m_is_partial_update)
		{
			return;
		}

		if(!g_server_started)
		{
			auto& l_script_system = m_game_system->get_script_system();
			l_script_system.run_script_throw(game::bc_script_context::app, L"server.start(6600);");

#ifdef BC_DEBUG
			l_script_system.run_script_throw(game::bc_script_context::app, L"server.load_scene('Test');");
#else
			l_script_system.run_script_throw(game::bc_script_context::app, L"server.load_scene('BattleGround');");
#endif

			g_server_started = true;
			return;
		}

		if(m_state == bx_app_state::initial)
		{
			return;
		}

		if(m_state == bx_app_state::game_started)
		{
			m_current_game_time -= p_context.m_clock.m_elapsed_second;

			if(m_current_game_time > 0)
			{
				_respawn_dead_players(p_context.m_clock);
			}
			else
			{
				_show_game_scores();
			}
		}

		if(m_state == bx_app_state::game_scores)
		{
			m_current_scores_time -= p_context.m_clock.m_elapsed_second;

			if(m_current_scores_time <= 0)
			{
				_reset_game_and_restore_checkpoint(*m_scene, p_context.m_clock);
			}
		}

		_send_game_state_to_clients(p_context.m_clock);
	}

	void bx_server_application::application_render(const bc_application_render_context& p_context)
	{
	}

	void bx_server_application::application_swap_frame(const platform::bc_clock::update_param& p_clock)
	{
		core::bc_estring_frame l_caption(m_app_name);
		l_caption += bcL(" ") + core::bc_to_estring_frame(get_fps(), L"%.1f");
		m_console->get_console_window()->set_caption(l_caption.c_str());
	}
	
	void bx_server_application::application_event(core::bci_event& p_event)
	{
		if(const auto* l_player_spawn_event = core::bci_message::as<bx_player_spawned_event>(p_event))
		{
			m_player_service->player_spawned(l_player_spawn_event->get_client_id(), l_player_spawn_event->get_actor());
			return;
		}

		if (const auto* l_player_kill_event = core::bci_message::as<bx_player_killed_event>(p_event))
		{
			auto [l_player, l_killer] = m_player_service->player_killed(l_player_kill_event->get_client_id(), l_player_kill_event->get_killer_client_id());
			
			bx_player_kill_state l_kill_state
			{
				*l_killer->m_team,
				l_killer->m_name,
				*l_player->m_team,
				l_player->m_name
			};
			m_killing_list.push_back(std::move(l_kill_state));

			return;
		}

		if (const auto* l_player_remove_event = core::bci_message::as<bx_player_removed_event>(p_event))
		{
			m_player_service->player_removed(l_player_remove_event->get_client_id());
			return;
		}

		if (const auto* l_close_event = core::bci_message::as<platform::bc_app_event_window_close>(p_event))
		{
			if (m_console->get_console_window()->get_id() == l_close_event->get_window_id())
			{
				auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();
				platform::bc_app_event_exit l_exit_event(0);
				l_event_manager->process_event(l_exit_event);
			}

			return;
		}
	}

	void bx_server_application::application_unload_content(const bc_application_load_context& p_context)
	{
		bx_unload_game_resources(p_context.m_stream_manager);
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

		m_player_spawn_event_handle.reset();
		m_player_kill_event_handle.reset();
		m_player_remove_event_handle.reset();
	}

	void bx_server_application::application_close_engine_components()
	{
		bx_close_game_services();
	}

	void bx_server_application::started_listening(bcUINT16 p_port) noexcept
	{
		m_state = bx_app_state::server_started;
	}

	void bx_server_application::scene_changed(game::bc_scene* p_scene) noexcept
	{
		m_scene = p_scene;
		m_player_service->scene_changed(m_scene);

		if(m_scene)
		{
			_create_scene_checkpoint(*m_scene);
			_reset_game(*m_scene);
		}
		else
		{
			m_state = bx_app_state::initial;
		}
	}

	core::bc_string bx_server_application::client_connected(const game::bc_network_client& p_client) noexcept
	{
		if(m_scene == nullptr)
		{
			return "Server has not loaded a scene yet";
		}

		const auto l_player_count = m_player_service->player_count();
		const auto l_scene_capacity = m_player_service->capacity();
		if(l_player_count >= l_scene_capacity)
		{
			return "Server is full";
		}

		m_player_service->join_player(p_client.m_address, p_client.m_id, core::bc_string(p_client.m_name));

		const auto l_message = core::bc_string_stream() << "'" << p_client.m_name << "'" << " joined the game.";
		m_info_messages.push_back(l_message.str());

		return {};
	}

	void bx_server_application::client_disconnected(const game::bc_network_client& p_client) noexcept
	{
		const auto l_message = core::bc_string_stream() << "'" << p_client.m_name << "'" << " left the game.";
		m_info_messages.push_back(l_message.str());

		m_player_service->disconnect_player(p_client.m_id);
	}

	void bx_server_application::message_packet_sent(const game::bc_network_client& p_client, 
		const core::bc_memory_stream& p_packet, 
		bcSIZE p_packet_size, 
		core::bc_const_span<game::bc_network_message_ptr> p_messages) noexcept
	{
		/*const core::bc_string_frame l_packet(static_cast<const bcCHAR*>(p_packet.get_position_data()), p_packet_size);
		core::bc_log(core::bc_log_type::debug) << core::bc_only_file << "Network packet sent to client: " << p_client << " " << l_packet << core::bc_lend;*/
	}

	void bx_server_application::message_packet_received(const game::bc_network_client& p_client, 
		const core::bc_memory_stream& p_packet, 
		bcSIZE p_packet_size, 
		core::bc_const_span<game::bc_network_message_ptr> p_messages) noexcept
	{
		/*const core::bc_string_frame l_packet(static_cast<const bcCHAR*>(p_packet.get_position_data()), p_packet_size);
		core::bc_log(core::bc_log_type::debug) << core::bc_only_file << "Network packet received from client: " << p_client << " " << l_packet << core::bc_lend;*/
	}

	void bx_server_application::error_occurred(const game::bc_network_client* p_client, const bc_network_exception* p_exception) noexcept
	{
		if(!p_client)
		{
			m_state = bx_app_state::initial;
			m_game_system->set_scene(nullptr);
		}
	}

	core::bc_string bx_server_application::change_player_team(game::bc_network_client_id p_client_id, bx_team p_team)
	{
		const auto* l_player = m_player_service->find_player(p_client_id);
		const auto l_seat = m_player_service->assign_seat(p_client_id, p_team);

		if(!l_seat.first)
		{
			return "Team is full";
		}

		if(m_state == bx_app_state::game_started)
		{
			m_network_system->send_message(l_player->m_address, bx_player_spawn_network_message(l_seat.second, p_team));
		}

		return {};
	}

	void bx_server_application::message_received(game::bc_network_client_id p_client_id, core::bc_string p_msg)
	{
		const auto* l_player = m_player_service->find_player(p_client_id);
		const auto l_client_name = core::bc_string(l_player->m_name) + ": ";
		p_msg.insert(0, l_client_name);

		m_network_system->send_message(bx_game_message_network_message(p_msg));
	}

	void bx_server_application::_send_game_state_to_clients(const platform::bc_clock::update_param& p_clock)
	{
		m_last_state_update_elapsed_ms += p_clock.m_elapsed;

		if(m_last_state_update_elapsed_ms >= 500)
		{
			m_last_state_update_elapsed_ms = 0;
			
			const bx_game_state l_game_state
			{
				static_cast<bcUINT32>(m_current_game_time),
				m_player_service->get_scores(),
				std::move(m_info_messages),
				std::move(m_killing_list)
			};
			m_network_system->send_message(bx_game_state_network_message(l_game_state));
		}
	}

	void bx_server_application::_create_scene_checkpoint(game::bc_scene& p_scene)
	{
		auto& l_content_manager = *core::bc_get_service<core::bc_content_manager>();
		const auto l_checkpoint_path = bx_scene_checkpoint::get_checkpoint_path(p_scene, bcL("server_checkpoint"));

		bx_scene_checkpoint l_check_point(p_scene);
		l_content_manager.save_as(l_check_point, l_checkpoint_path.get_string_frame().c_str(), {});
	}

	void bx_server_application::_restore_scene_checkpoint(game::bc_scene& p_scene, const platform::bc_clock::update_param& p_clock)
	{
		auto& l_content_manager = *core::bc_get_service<core::bc_content_manager>();
		const auto l_checkpoint_path = bx_scene_checkpoint::get_checkpoint_path(p_scene, bcL("server_checkpoint"));
		auto l_checkpoint_params = core::bc_content_loader_parameter();
		l_checkpoint_params.add_or_update("params", core::bc_any(bc_scene_checkpoint_loader_params{ &p_clock, &p_scene }));

		auto l_check_point = l_content_manager.load<bx_scene_checkpoint>
		(
			l_checkpoint_path.get_string_frame().c_str(),
			{},
			{},
			std::move(l_checkpoint_params)
		);
	}

	void bx_server_application::_show_game_scores()
	{
		m_state = bx_app_state::game_scores;
		m_network_system->send_message(bx_game_scores_network_message());
	}

	void bx_server_application::_reset_game(game::bc_scene& p_scene)
	{
		m_current_game_time = m_game_time;
		m_current_scores_time = m_scores_time;
		m_state = bx_app_state::game_started;
		m_player_service->reset_players();
		
		m_network_system->send_message(bx_game_reset_network_message());
	}

	void bx_server_application::_reset_game_and_restore_checkpoint(game::bc_scene& p_scene, const platform::bc_clock::update_param& p_clock)
	{
		_reset_game(p_scene);
		_restore_scene_checkpoint(p_scene, p_clock);
	}

	void bx_server_application::_respawn_dead_players(const platform::bc_clock::update_param& p_clock)
	{
		// Game is gonna end
		if(m_current_game_time <= 1)
		{
			return;
		}

		const auto l_respawned_players = m_player_service->respawn_dead_players(p_clock, m_respawn_time);

		for (const auto l_player : l_respawned_players)
		{
			m_network_system->send_message(l_player->m_address, bx_player_spawn_network_message(l_player->m_seat, *l_player->m_team));
		}
	}
}
