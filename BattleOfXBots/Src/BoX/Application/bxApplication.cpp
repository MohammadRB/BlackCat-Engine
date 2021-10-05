// [06/20/2021 MRB]

#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Utility/bcLogger.h"
#include "Core/bcUtility.h"
#include "Platform/bcEvent.h"
#include "PlatformImp/Script/bcScriptGlobalPrototypeBuilder.h"
#include "PlatformImp/Script/bcScriptPrototypeBuilder.h"
#include "Game/System/Script/bcScriptBinding.h"
#include "Game/System/Script/bcScriptSystem.h"
#include "Game/System/Input/bcFreeCamera.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/Object/Mesh/bcHeightMap.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"
#include "BlackCat/Loader/bcHeightMapLoaderDx11.h"
#include "BlackCat/RenderPass/bcBackBufferWritePass.h"
#include "BlackCat/RenderPass/bcShapeDrawPass.h"
#include "BlackCat/RenderPass/bcTextDrawPass.h"
#include "BlackCat/RenderPass/GBuffer/bcGBufferDecalPass.h"
#include "BlackCat/RenderPass/GBuffer/bcGBufferInitializePass.h"
#include "BlackCat/RenderPass/GBuffer/bcGBufferLightMapPass.h"
#include "BlackCat/RenderPass/GBuffer/bcGBufferPass.h"
#include "BlackCat/RenderPass/GBuffer/bcGBufferSkinnedPass.h"
#include "BlackCat/RenderPass/GBuffer/bcGBufferTerrainPassDx11.h"
#include "BlackCat/RenderPass/GBuffer/bcGBufferVegetablePass.h"
#include "BlackCat/RenderPass/PostProcess/bcLightFlarePass.h"
#include "BlackCat/RenderPass/PostProcess/bcParticleSystemPassDx11.h"
#include "BlackCat/RenderPass/ShadowMap/bcCascadedShadowMapPass.h"
#include "BlackCat/RenderPass/ShadowMap/bcSkinnedCascadedShadowMapPass.h"
#include "BlackCat/RenderPass/ShadowMap/bcVegetableCascadedShadowMapPass.h"
#include "BlackCat/bcConstant.h"
#include "BoX/Application/bxApplication.h"
#include "BoX/Application/bxClientScript.h"

namespace box
{
	void bx_application::bind(platform::bc_script_context& p_context, platform::bc_script_global_prototype_builder& p_global_prototype, bx_application& p_instance)
	{
		{
			platform::bc_script_context::scope l_scope(p_context);

			auto l_client_prototype_builder = p_context.create_prototype_builder<bx_client_script>();
			l_client_prototype_builder
				.function(L"connect", &bx_client_script::connect)
				.function(L"send", &bx_client_script::send);

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
		core::bc_register_loader<game::bc_height_map, bc_height_map_loader_dx11>
		(
			"height_map", core::bc_make_loader<bc_height_map_loader_dx11>()
		);
	}

	void bx_application::application_initialize(game::bc_engine_application_parameter& p_parameters)
	{
		auto& l_game_console = m_game_system->get_console();
		auto& l_input_system = m_game_system->get_input_system();
		auto& l_render_system = m_game_system->get_render_system();
		auto& l_script_system = m_game_system->get_script_system();
		auto& l_global_config = get_global_config();
		
		m_console = core::bc_make_unique<game::bc_default_game_console>(*this, l_game_console);
		m_console->show();
		m_console->get_console_window()->set_caption(p_parameters.m_app_parameters.m_app_name);
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
		l_render_system.add_render_pass(bc_cascaded_shadow_map_pass(constant::g_rpass_direct_light_depth_buffers, 2, { {30, 1}, {60, 1}, {120, 2} }));
		l_render_system.add_render_pass(bc_vegetable_cascaded_shadow_map_pass(*l_render_system.get_render_pass<bc_cascaded_shadow_map_pass>()));
		l_render_system.add_render_pass(bc_skinned_cascaded_shadow_map_pass(*l_render_system.get_render_pass<bc_cascaded_shadow_map_pass>()));
		l_render_system.add_render_pass(bc_gbuffer_light_map_pass(constant::g_rpass_direct_light_depth_buffers, constant::g_rpass_deferred_rendering_g_buffer_output));
		l_render_system.add_render_pass(bc_back_buffer_write_pass(constant::g_rpass_deferred_rendering_g_buffer_output));
		l_render_system.add_render_pass(bc_shape_draw_pass(constant::g_rpass_back_buffer_view));
		l_render_system.add_render_pass(bc_particle_system_pass_dx11(bcL("Sprites.dds")));
		l_render_system.add_render_pass(bc_light_flare_pass());
		l_render_system.add_render_pass(bc_text_draw_pass(constant::g_rpass_back_buffer_view));
		
		l_script_system.get_script_binder().bind(game::bc_script_context::app, *this);
	}

	void bx_application::application_load_content(core::bc_content_stream_manager& p_stream_manager)
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
				auto& l_script_system = m_game_system->get_script_system();
				l_script_system.run_script_throw(game::bc_script_context::app, L"client.connect(\"127.0.0.1\", 6699);");
			}
		}
#endif
	}

	void bx_application::application_render(core_platform::bc_clock::update_param p_clock)
	{
	}

	bool bx_application::application_event(core::bci_event& p_event)
	{
		auto* l_key_event = core::bci_message::as<platform::bc_app_event_key>(p_event);
		if(l_key_event)
		{
			if (l_key_event->get_key_state() == platform::bc_key_state::pressing && l_key_event->get_key() == platform::bc_key::kb_space)
			{
				auto& l_input_system = m_game_system->get_input_system();
				auto& l_camera = *l_input_system.get_camera();
				auto* l_scene = m_game_system->get_scene();

				game::bc_actor l_actor = l_scene->create_actor("box-client", core::bc_matrix4f::translation_matrix(l_camera.get_position()));

				auto* l_rigid_component = l_actor.get_component<game::bc_rigid_body_component>();
				auto l_rigid = l_rigid_component->get_body();
				if (l_rigid.is_rigid_dynamic().is_valid())
				{
					const auto l_direction = l_camera.get_forward();
					l_rigid.set_linear_velocity(l_direction * 70);
				}
			}
		}
		
		auto* l_exit_event = core::bci_message::as<platform::bc_app_event_exit>(p_event);
		if (l_exit_event)
		{
			auto& l_global_config = get_global_config();
			auto* l_camera = m_game_system->get_input_system().get_camera();
			if(!l_camera)
			{
				return true;
			}
			
			const auto l_camera_position = l_camera->get_position();
			const auto l_camera_target = l_camera->get_look_at();

			l_global_config.add_or_update_config_key("camera_position", l_camera_position)
				.add_or_update_config_key("camera_lookat", l_camera_target)
				.flush_changes();
		}
		
		return false;
	}

	void bx_application::application_unload_content(core::bc_content_stream_manager& p_stream_manager)
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
		m_game_system->get_console().set_implementation(nullptr);
		m_console.reset();
	}

	void bx_application::application_close_engine_components()
	{
	}

	void bx_application::connecting_to_server(const platform::bc_network_address& p_address)
	{	
	}

	void bx_application::connected_to_server(const platform::bc_network_address& p_address)
	{
	}

	void bx_application::message_packet_sent(const core::bc_memory_stream& p_packet, bcSIZE p_packet_size, core::bc_const_span<game::bc_network_message_ptr> p_messages)
	{
		//const core::bc_string_frame l_packet(static_cast<const bcCHAR*>(p_packet.get_position_data()), p_packet_size);
		//core::bc_log(core::bc_log_type::debug) << core::bc_only_file << "Network packet sent to server: " << l_packet << core::bc_lend;
	}

	void bx_application::message_packet_received(const core::bc_memory_stream& p_packet, bcSIZE p_packet_size, core::bc_const_span<game::bc_network_message_ptr> p_messages)
	{
		//const core::bc_string_frame l_packet(static_cast<const bcCHAR*>(p_packet.get_position_data()), p_packet_size);
		//core::bc_log(core::bc_log_type::debug) << core::bc_only_file << "Network packet received from server: " << l_packet << core::bc_lend;
	}

	void bx_application::error_occurred(const bc_network_exception* p_exception)
	{
	}
}