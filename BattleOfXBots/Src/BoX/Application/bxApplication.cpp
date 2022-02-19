// [06/20/2021 MRB]

#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Content/bcContentManager.h"
#include "Core/Utility/bcLogger.h"
#include "Core/bcUtility.h"
#include "Platform/bcEvent.h"
#include "PlatformImp/Script/bcScriptGlobalPrototypeBuilder.h"
#include "PlatformImp/Script/bcScriptPrototypeBuilder.h"
#include "Game/System/Script/bcScriptBinding.h"
#include "Game/System/Script/bcScriptSystem.h"
#include "Game/System/Input/bcFreeCamera.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/Object/Mesh/bcHeightMap.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"
#include "Game/Object/Scene/bcSceneCheckPoint.h"
#include "App/RenderPass/bcBackBufferWritePass.h"
#include "App/RenderPass/bcShapeDrawPass.h"
#include "App/RenderPass/bcTextDrawPass.h"
#include "App/RenderPass/GBuffer/bcGBufferDecalPass.h"
#include "App/RenderPass/GBuffer/bcGBufferInitializePass.h"
#include "App/RenderPass/GBuffer/bcGBufferLightMapPass.h"
#include "App/RenderPass/GBuffer/bcGBufferPass.h"
#include "App/RenderPass/GBuffer/bcGBufferSkinnedPass.h"
#include "App/RenderPass/GBuffer/bcGBufferTerrainPassDx11.h"
#include "App/RenderPass/GBuffer/bcGBufferVegetablePass.h"
#include "App/RenderPass/PostProcess/bcLightFlarePass.h"
#include "App/RenderPass/PostProcess/bcGlowPass.h"
#include "App/RenderPass/PostProcess/bcParticleSystemPassDx11.h"
#include "App/RenderPass/ShadowMap/bcCascadedShadowMapPass.h"
#include "App/RenderPass/ShadowMap/bcSkinnedCascadedShadowMapPass.h"
#include "App/RenderPass/ShadowMap/bcVegetableCascadedShadowMapPass.h"
#include "App/bcConstant.h"
#include "BoX.Game/Application/bxApplicationHookFunctions.h"
#include "BoX.Game/RenderPass/bxPlayerUIPass.h"
#include "BoX.Game/Network/bxTeamSelectNetworkMessage.h"
#include "BoX.Game/bxEvent.h"
#include "BoX/Application/bxApplication.h"
#include "BoX/Application/bxClientScript.h"

namespace box
{
	constexpr bcFLOAT g_client_start_delay = 1.0f;
	core_platform::bc_clock::small_delta_time g_client_start_time = 0;

	void bx_application::bind(platform::bc_script_context& p_context, platform::bc_script_global_prototype_builder& p_global_prototype, bx_application& p_instance)
	{
		{
			platform::bc_script_context::scope l_scope(p_context);

			auto l_client_prototype_builder = p_context.create_prototype_builder<bx_client_script>();
			l_client_prototype_builder
				.function(L"connect", &bx_client_script::connect)
				.function(L"send", &bx_client_script::send);

			auto l_client_prototype = p_context.create_prototype(l_client_prototype_builder);
			const auto l_client_object = p_context.create_object(l_client_prototype, bx_client_script(*p_instance.m_game_system, p_instance, p_instance));
			
			p_instance.m_client_script_context = &p_context;
			p_instance.m_client_script_object.reset(l_client_object);

			platform::bc_script_property_descriptor<platform::bc_script_object> l_client_property(&p_instance.m_client_script_object.get(), false);
			p_global_prototype.property(L"client", l_client_property);
		}
	}

	void bx_application::application_start_engine_components(const bc_application_start_context& p_context)
	{
		bx_start_game_services(p_context);
		bx_register_game_loaders(p_context);
		bx_register_game_actor_components();
		bx_register_game_network_messages(m_game_system->get_network_system());
		bx_bind_game_scripts(*m_game_system);
		bx_register_game_particle_emitters(*m_game_system);
		bx_load_game_shaders(*core::bc_get_service<core::bc_content_stream_manager>(), *m_game_system);

		m_player_service = core::bc_get_service<bx_player_service>();
	}

	void bx_application::application_initialize(const bc_application_initialize_context& p_context)
	{
		auto& l_event_manager = *core::bc_get_service<core::bc_event_manager>();
		auto& l_game_console = m_game_system->get_console();
		auto& l_input_system = m_game_system->get_input_system();
		auto& l_file_system = m_game_system->get_file_system();
		auto& l_render_system = m_game_system->get_render_system();
		auto& l_script_system = m_game_system->get_script_system();
		auto& l_global_config = bc_get_global_config();
		
		m_console = core::bc_make_unique<game::bc_default_game_console>(*this, l_game_console);
		m_console->show();
		m_console->get_console_window()->set_caption(p_context.m_app_parameters.m_app_name);
		l_game_console.set_implementation(m_console.get());

		bool l_camera_read = false;
		core::bc_vector3f l_camera_pos;
		core::bc_vector3f l_camera_look_at;
		l_global_config.read_config_key("camera_position", l_camera_pos, l_camera_read);
		l_global_config.read_config_key("camera_lookat", l_camera_look_at, l_camera_read);
		
		auto l_camera = core::bc_make_unique<game::bc_free_camera>
		(
			l_render_system.get_device_swap_buffer().get_back_buffer_width(),
			l_render_system.get_device_swap_buffer().get_back_buffer_height(),
			1.2,
			0.3,
			3000
		);
		l_camera->set_look_at(l_camera_pos, l_camera_look_at);
		l_input_system.add_camera(std::move(l_camera));

		l_render_system.add_render_pass(bc_gbuffer_initialize_pass());
		l_render_system.add_render_pass(bc_gbuffer_terrain_pass_dx11());
		l_render_system.add_render_pass(bc_gbuffer_pass());
		l_render_system.add_render_pass(bc_gbuffer_vegetable_pass());
		l_render_system.add_render_pass(bc_gbuffer_skinned_pass());
		l_render_system.add_render_pass(bc_gbuffer_decal_pass());
		l_render_system.add_render_pass(bc_cascaded_shadow_map_pass(constant::g_rpass_direct_light_depth_buffers, 3, { {50, 1}, {100, 1} }));
		l_render_system.add_render_pass(bc_vegetable_cascaded_shadow_map_pass(*l_render_system.get_render_pass<bc_cascaded_shadow_map_pass>()));
		l_render_system.add_render_pass(bc_skinned_cascaded_shadow_map_pass(*l_render_system.get_render_pass<bc_cascaded_shadow_map_pass>()));
		l_render_system.add_render_pass(bc_gbuffer_light_map_pass(constant::g_rpass_direct_light_depth_buffers, constant::g_rpass_deferred_rendering_g_buffer_output));
		l_render_system.add_render_pass(bc_back_buffer_write_pass(constant::g_rpass_deferred_rendering_g_buffer_output));
		l_render_system.add_render_pass(bc_particle_system_pass_dx11(constant::g_rpass_back_buffer_texture, constant::g_rpass_back_buffer_render_view, bcL("Sprites.dds")));
		l_render_system.add_render_pass(bc_light_flare_pass(constant::g_rpass_back_buffer_texture, constant::g_rpass_back_buffer_render_view));
		l_render_system.add_render_pass(bc_glow_pass(constant::g_rpass_back_buffer_texture, constant::g_rpass_back_buffer_render_view));
		l_render_system.add_render_pass(bc_shape_draw_pass(constant::g_rpass_back_buffer_render_view));
		l_render_system.add_render_pass(bc_text_draw_pass(constant::g_rpass_back_buffer_render_view, l_file_system.get_content_data_path(bcL("Dx.spritefont"))));
		l_render_system.add_render_pass(bc_player_ui_pass(constant::g_rpass_back_buffer_texture, constant::g_rpass_back_buffer_render_view, l_file_system.get_content_data_path(bcL("Dx.spritefont"))));

		m_player_spawn_event_handle = l_event_manager.register_event_listener<bx_player_spawned_event>
		(
			core::bc_event_manager::delegate_type(*this, &bx_application::application_event)
		);
		m_player_kill_event_handle = l_event_manager.register_event_listener<bx_player_killed_event>
		(
			core::bc_event_manager::delegate_type(*this, &bx_application::application_event)
		);
		m_player_remove_event_handle = l_event_manager.register_event_listener<bx_player_removed_event>
		(
			core::bc_event_manager::delegate_type(*this, &bx_application::application_event)
		);

		l_script_system.get_script_binder().bind(game::bc_script_context::app, *this);
	}

	void bx_application::application_load_content(const bc_application_load_context& p_context)
	{
		bx_load_game_resources(p_context.m_stream_manager, *m_game_system);
	}

	void bx_application::application_update(const bc_application_update_context& p_context)
	{
		if(g_client_start_time <= g_client_start_delay && !p_context.m_is_partial_update)
		{
			g_client_start_time += p_context.m_clock.m_elapsed_second;
			if(g_client_start_time > g_client_start_delay)
			{
				auto& l_script_system = m_game_system->get_script_system();
				l_script_system.run_script_throw(game::bc_script_context::app, L"client.connect('127.0.0.1', 6699);");
			}

			return;
		}

		m_player_service->set_game_time(m_current_game_time);

		if(m_state == bx_app_state::scene_loaded)
		{
			if(m_team_select_task.valid() && m_team_select_task.is_ready())
			{
				const auto l_team = m_team_select_task.get();
				m_game_system->get_network_system().send_message(bx_team_select_network_message(l_team));
			}
			return;
		}

		if (m_state == bx_app_state::game_started)
		{
			if(m_is_dead)
			{
				m_dead_passed_time += p_context.m_clock.m_elapsed_second;
				if(m_dead_passed_time >= 4)
				{
					m_is_dead = false;
					m_dead_passed_time = 0;

					m_scene->remove_actor(m_player_actor);
				}
			}

			if (m_current_game_time <= 0)
			{
				_reset_game(*m_scene);
			}
		}
	}

	void bx_application::application_render(const bc_application_render_context& p_context)
	{
	}

	void bx_application::application_event(core::bci_event& p_event)
	{
		if (const auto* l_player_spawn_event = core::bci_message::as<bx_player_spawned_event>(p_event))
		{
			if(!l_player_spawn_event->is_local_player())
			{
				return;
			}

			m_is_dead = false;
			m_player_actor = l_player_spawn_event->get_actor();
			return;
		}

		if (const auto* l_player_kill_event = core::bci_message::as<bx_player_killed_event>(p_event))
		{
			if (!l_player_kill_event->is_local_player())
			{
				return;
			}

			m_is_dead = true;
			return;
		}

		if (const auto* l_player_remove_event = core::bci_message::as<bx_player_removed_event>(p_event))
		{
			if (!l_player_remove_event->is_local_player())
			{
				return;
			}

			m_is_dead = false;
			m_player_actor = game::bc_actor();
			m_player_service->stopped_playing();
			return;
		}

		if (const auto* l_exit_event = core::bci_message::as<platform::bc_app_event_exit>(p_event))
		{
			auto& l_global_config = bc_get_global_config();
			auto* l_camera = m_game_system->get_input_system().get_camera();
			if(!l_camera)
			{
				return;
			}
			
			const auto l_camera_position = l_camera->get_position();
			const auto l_camera_target = l_camera->get_look_at();

			l_global_config.add_or_update_config_key("camera_position", l_camera_position)
				.add_or_update_config_key("camera_lookat", l_camera_target)
				.flush_changes();
		}
	}

	void bx_application::application_unload_content(const bc_application_load_context& p_context)
	{
		bx_unload_game_resources(p_context.m_stream_manager);
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
		m_game_system->get_console().set_implementation(nullptr);
		m_console.reset();

		m_player_spawn_event_handle.reset();
		m_player_kill_event_handle.reset();
		m_player_remove_event_handle.reset();
	}

	void bx_application::application_close_engine_components()
	{
		bx_close_game_services();
	}

	void bx_application::connecting_to_server(const platform::bc_network_address& p_server_address) noexcept
	{
	}

	void bx_application::connection_to_server_approved(const platform::bc_network_address& p_server_address, const game::bc_client_connect_result& p_result) noexcept
	{
		if(p_result.m_error_message.empty())
		{
			m_state = bx_app_state::client_started;
		}
	}

	void bx_application::scene_changed(game::bc_scene* p_scene) noexcept
	{
		m_state = bx_app_state::scene_loaded;
		m_scene = p_scene;
		m_team_select_task = m_player_service->ask_for_team();

		_create_scene_checkpoint(*m_scene);
	}

	void bx_application::message_packet_sent(const core::bc_memory_stream& p_packet, bcSIZE p_packet_size, core::bc_const_span<game::bc_network_message_ptr> p_messages) noexcept
	{
		/*const core::bc_string_frame l_packet(static_cast<const bcCHAR*>(p_packet.get_position_data()), p_packet_size);
		core::bc_log(core::bc_log_type::debug) << core::bc_only_file << "Network packet sent to server: " << l_packet << core::bc_lend;*/
	}

	void bx_application::message_packet_received(const core::bc_memory_stream& p_packet, bcSIZE p_packet_size, core::bc_const_span<game::bc_network_message_ptr> p_messages) noexcept
	{
		/*const core::bc_string_frame l_packet(static_cast<const bcCHAR*>(p_packet.get_position_data()), p_packet_size);
		core::bc_log(core::bc_log_type::debug) << core::bc_only_file << "Network packet received from server: " << l_packet << core::bc_lend;*/
	}

	void bx_application::error_occurred(const bc_network_exception* p_exception) noexcept
	{
		m_game_system->set_scene(nullptr);
	}

	void bx_application::update_game_state(const bx_game_state& p_state)
	{
		m_current_game_time = p_state.m_game_time;
	}

	void bx_application::team_change_rejected(core::bc_string p_error)
	{
		m_player_service->add_error(core::bc_to_wstring(p_error));
		m_team_select_task = m_player_service->ask_for_team();
	}

	void bx_application::spawn_player(const core::bc_vector3f& p_position, bx_team p_team)
	{
		m_spawn_position = p_position;
		m_team = p_team;

		const bcCHAR* l_entity_name = p_team == bx_team::red ? "box_red_player" : "box_blue_player";
		m_player_actor = m_game_system->get_scene()->create_actor(l_entity_name, core::bc_matrix4f::translation_matrix(p_position));

		m_state = bx_app_state::game_started;
		m_player_service->started_playing(m_team);
	}

	void bx_application::_create_scene_checkpoint(game::bc_scene& p_scene)
	{
		auto& l_content_manager = *core::bc_get_service<core::bc_content_manager>();
		const auto l_checkpoint_path = game::bc_scene_check_point::get_checkpoint_path(p_scene, bcL("game_checkpoint"));

		game::bc_scene_check_point l_check_point(p_scene);
		l_content_manager.save_as(l_check_point, l_checkpoint_path.get_string_frame().c_str(), nullptr);
	}

	void bx_application::_restore_scene_checkpoint(game::bc_scene& p_scene)
	{
		auto& l_content_manager = *core::bc_get_service<core::bc_content_manager>();
		const auto l_checkpoint_path = game::bc_scene_check_point::get_checkpoint_path(p_scene, bcL("game_checkpoint"));

		auto l_check_point = l_content_manager.load<game::bc_scene_check_point>
		(
			l_checkpoint_path.get_string_frame().c_str(),
			nullptr,
			core::bc_content_loader_parameter(),
			core::bc_content_loader_parameter().add_or_update("scene", &p_scene)
		);
	}

	void bx_application::_reset_game(game::bc_scene& p_scene)
	{
		m_current_game_time = 0;
		m_state = bx_app_state::scene_loaded;
		m_team_select_task = m_player_service->ask_for_team();

		_restore_scene_checkpoint(p_scene);
	}
}
