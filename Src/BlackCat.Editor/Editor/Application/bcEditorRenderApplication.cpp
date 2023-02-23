// [11/13/2016 MRB]

#include "Editor/EditorPCH.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/bcUtility.h"
#include "Platform/bcEvent.h"
#include "Game/System/Input/bcInputSystem.h"
#include "Game/System/Input/bcFileSystem.h"
#include "Game/System/Input/bcFreeCamera.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/Object/Scene/Component/bcIconComponent.h"
#include "Game/bcEvent.h"
#include "Game/bcUtility.h"
#include "App/RenderPass/GBuffer/bcGBufferInitializePass.h"
#include "App/RenderPass/GBuffer/bcGBufferTerrainPassDx11.h"
#include "App/RenderPass/GBuffer/bcGBufferPass.h"
#include "App/RenderPass/GBuffer/bcGBufferVegetablePass.h"
#include "App/RenderPass/GBuffer/bcGBufferSkinnedPass.h"
#include "App/RenderPass/GBuffer/bcGBufferDecalPass.h"
#include "App/RenderPass/GBuffer/bcGBufferLightMapPass.h"
#include "App/RenderPass/ShadowMap/bcCascadedShadowMapPass.h"
#include "App/RenderPass/ShadowMap/bcVegetableCascadedShadowMapPass.h"
#include "App/RenderPass/ShadowMap/bcSkinnedCascadedShadowMapPass.h"
#include "App/RenderPass/PostProcess/bcParticleSystemPassDx11.h"
#include "App/RenderPass/PostProcess/bcLightFlarePass.h"
#include "App/RenderPass/PostProcess/bcGlowPass.h"
#include "App/RenderPass/PostProcess/bcEdgeDetectionAntiAliasingPass.h"
#include "App/RenderPass/PostProcess/bcHDRRenderingPass.h"
#include "App/RenderPass/bcIconDrawPass.h"
#include "App/RenderPass/bcShapeDrawPass.h"
#include "App/RenderPass/bcCounterValueDrawPass.h"
#include "App/RenderPass/bcBackBufferWritePass.h"
#include "App/bcConstant.h"
#include "Editor/Application/bcEditorHeightMapLoaderDx11.h"
#include "Editor/Application/bcEditorRenderApplication.h"
#include "Editor/Application/bcUICommandService.h"

namespace black_cat
{
	namespace editor
	{
		bc_editor_render_app::bc_editor_render_app() = default;

		bc_editor_render_app::~bc_editor_render_app() = default;

		void bc_editor_render_app::application_start_engine_components(const bc_application_start_context& p_context)
		{
			core::bc_register_service(core::bc_make_service<bc_ui_command_service>
			(
				*core::bc_get_service<core::bc_content_stream_manager>(),
				*core::bc_get_service<core::bc_event_manager>(),
				*m_game_system
			));
			core::bc_register_loader<game::bc_height_map, bc_editor_height_map_loader_dx11>
			(
				"height_map", core::bc_make_loader<bc_editor_height_map_loader_dx11>()
			);
		}

		void bc_editor_render_app::application_initialize(const bc_application_initialize_context& p_context)
		{
			auto& l_event_manager = *core::bc_get_service<core::bc_event_manager>();
			auto& l_render_system = m_game_system->get_render_system();
			auto& l_input_system = m_game_system->get_input_system();
			auto& l_global_config = bc_get_global_config();
			
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
			l_input_system.add_editor_camera(std::move(l_camera));
			
			l_render_system.add_render_pass(bc_gbuffer_initialize_pass());
			l_render_system.add_render_pass(bc_gbuffer_pass());
			l_render_system.add_render_pass(bc_gbuffer_vegetable_pass());
			l_render_system.add_render_pass(bc_gbuffer_skinned_pass());
			l_render_system.add_render_pass(bc_gbuffer_terrain_pass_dx11());
			l_render_system.add_render_pass(bc_gbuffer_decal_pass());
			l_render_system.add_render_pass(bc_cascaded_shadow_map_pass(constant::g_rpass_direct_light_depth_buffers, 2, { {40, 1}, {70, 1}, {130, 2} }));
			l_render_system.add_render_pass(bc_vegetable_cascaded_shadow_map_pass(*l_render_system.get_render_pass<bc_cascaded_shadow_map_pass>()));
			l_render_system.add_render_pass(bc_skinned_cascaded_shadow_map_pass(*l_render_system.get_render_pass<bc_cascaded_shadow_map_pass>()));
			l_render_system.add_render_pass(bc_gbuffer_light_map_pass
			(
				constant::g_rpass_direct_light_depth_buffers, 
				constant::g_rpass_deferred_rendering_gbuffer_texture,
				constant::g_rpass_deferred_rendering_gbuffer_read_view,
				constant::g_rpass_deferred_rendering_gbuffer_render_view
			));
			l_render_system.add_render_pass(bc_particle_system_pass_dx11
			(
				constant::g_rpass_deferred_rendering_gbuffer_texture,
				constant::g_rpass_deferred_rendering_gbuffer_render_view,
				bcL("Texture\\Particle\\Particle.dds")
			));
			l_render_system.add_render_pass(bc_light_flare_pass(constant::g_rpass_deferred_rendering_gbuffer_texture, constant::g_rpass_deferred_rendering_gbuffer_render_view));
			l_render_system.add_render_pass(bc_hdr_rendering_pass
			(
				constant::g_rpass_deferred_rendering_gbuffer_texture,
				constant::g_rpass_deferred_rendering_gbuffer_read_view,
				constant::g_rpass_deferred_rendering_gbuffer_render_view,
				constant::g_rpass_hdr_output_texture,
				constant::g_rpass_hdr_output_texture_read_view,
				constant::g_rpass_hdr_output_texture_render_view
			));
			l_render_system.add_render_pass(bc_glow_pass(constant::g_rpass_hdr_output_texture, constant::g_rpass_hdr_output_texture_render_view));
			l_render_system.add_render_pass(bc_back_buffer_write_pass(constant::g_rpass_hdr_output_texture));
			l_render_system.add_render_pass(bc_edge_detection_anti_aliasing_pass
			(
				constant::g_rpass_hdr_output_texture_read_view,
				constant::g_rpass_back_buffer_texture, 
				constant::g_rpass_back_buffer_render_view
			));
			l_render_system.add_render_pass(bc_icon_draw_pass
			(
				constant::g_rpass_back_buffer_texture, 
				constant::g_rpass_back_buffer_render_view, 
				{
					game::bc_icon_type::player,
					game::bc_icon_type::sun,
					game::bc_icon_type::light,
					game::bc_icon_type::cloud,
					game::bc_icon_type::wind,
					game::bc_icon_type::sound,
					game::bc_icon_type::particle,
					game::bc_icon_type::decal
				},
				bcL("Texture\\SpriteIcon.png"), 
				64, 
				64
			));
			l_render_system.add_render_pass(bc_shape_draw_pass(constant::g_rpass_back_buffer_render_view));
			l_render_system.add_render_pass(bc_counter_value_draw_pass(constant::g_rpass_back_buffer_texture, constant::g_rpass_back_buffer_render_view, bcL("Data\\Dx.spritefont")));

			game::bc_event_editor_mode l_editor_mode_event(true);
			l_event_manager.process_event(l_editor_mode_event);
		}

		void bc_editor_render_app::application_load_content(const bc_application_load_context& p_context)
		{
			core::bc_get_service<bc_ui_command_service>()->load_content();
		}

		void bc_editor_render_app::application_update(const bc_application_update_context& p_context)
		{
		}

		void bc_editor_render_app::application_render(const bc_application_render_context& p_context)
		{
		}

		void bc_editor_render_app::application_event(core::bci_event& p_event)
		{
			if (const auto* l_key_event = core::bci_message::as<platform::bc_app_event_key>(p_event))
			{
				if (l_key_event->get_key_state() == platform::bc_key_state::pressing && l_key_event->get_key() == platform::bc_key::kb_space)
				{
					auto& l_input_system = m_game_system->get_input_system();
					auto& l_camera = *l_input_system.get_camera();
					auto* l_scene = m_game_system->get_scene();

					//m_game_system->get_render_system().get_render_pass<bc_cascaded_shadow_map_pass>()->capture_debug_shapes();
				}
				
				return;
			}

			if (const auto* l_exit_event = core::bci_message::as<platform::bc_app_event_exit>(p_event))
			{
				auto& l_global_config = bc_get_global_config();
				auto* l_camera = m_game_system->get_input_system().get_camera();
				if (!l_camera)
				{
					return;
				}

				const auto l_camera_position = l_camera->get_position();
				const auto l_camera_target = l_camera->get_look_at();

				l_global_config.add_or_update_config_key("camera_position", l_camera_position)
					.add_or_update_config_key("camera_lookat", l_camera_target)
					.flush_changes();

				return;
			}
		}

		void bc_editor_render_app::application_unload_content(const bc_application_load_context& p_context)
		{
			core::bc_get_service<bc_ui_command_service>()->unload_content();
		}

		void bc_editor_render_app::application_destroy()
		{
		}

		void bc_editor_render_app::application_close_engine_components()
		{
		}
	}
}