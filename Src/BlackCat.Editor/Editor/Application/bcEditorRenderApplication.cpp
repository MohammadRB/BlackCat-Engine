// [11/13/2016 MRB]

#include "Editor/EditorPCH.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/bcUtility.h"
#include "Platform/bcEvent.h"
#include "PhysicsImp/Body/bcRigidDynamic.h"
#include "Game/System/Input/bcFreeCamera.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/bcEvent.h"
#include "Game/bcUtility.h"
#include "BlackCat/bcConstant.h"
#include "BlackCat/RenderPass/bcShapeDrawPass.h"
#include "BlackCat/RenderPass/GBuffer/bcGBufferInitializePass.h"
#include "BlackCat/RenderPass/GBuffer/bcGBufferTerrainPassDx11.h"
#include "BlackCat/RenderPass/GBuffer/bcGBufferPass.h"
#include "BlackCat/RenderPass/GBuffer/bcGBufferVegetablePass.h"
#include "BlackCat/RenderPass/GBuffer/bcGBufferSkinnedPass.h"
#include "BlackCat/RenderPass/GBuffer/bcGBufferDecalPass.h"
#include "BlackCat/RenderPass/GBuffer/bcGBufferLightMapPass.h"
#include "BlackCat/RenderPass/ShadowMap/bcCascadedShadowMapPass.h"
#include "BlackCat/RenderPass/ShadowMap/bcVegetableCascadedShadowMapPass.h"
#include "BlackCat/RenderPass/ShadowMap/bcSkinnedCascadedShadowMapPass.h"
#include "BlackCat/RenderPass/PostProcess/bcParticleSystemPassDx11.h"
#include "BlackCat/RenderPass/PostProcess/bcLightFlarePass.h"
#include "BlackCat/RenderPass/bcBackBufferWritePass.h"
#include "BlackCat/RenderPass/bcTextDrawPass.h"
#include "Editor/Application/bcEditorHeightMapLoaderDx11.h"
#include "Editor/Application/bcEditorRenderApplication.h"
#include "Editor/Application/bcUICommandService.h"

namespace black_cat
{
	namespace editor
	{
		bc_editor_render_app::bc_editor_render_app() = default;

		bc_editor_render_app::~bc_editor_render_app() = default;

		void bc_editor_render_app::application_start_engine_components(game::bc_engine_application_parameter& p_parameters)
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

		void bc_editor_render_app::application_initialize(game::bc_engine_application_parameter& p_parameters)
		{
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

			game::bc_event_editor_mode l_editor_mode_event(true);
			core::bc_get_service<core::bc_event_manager>()->process_event(l_editor_mode_event);
		}

		void bc_editor_render_app::application_load_content(core::bc_content_stream_manager& p_stream_manager)
		{
			core::bc_get_service<bc_ui_command_service>()->load_content();

			auto* l_content_manager = core::bc_get_service<core::bc_content_manager>();
			auto& l_file_system = m_game_system->get_file_system();

			const auto l_scene = l_content_manager->load<game::bc_scene>
			(
				l_file_system.get_content_scene_path(bcL("Test.json")).c_str(),
				nullptr,
				core::bc_content_loader_parameter()
			);

			m_game_system->set_scene(l_scene);
		}

		void bc_editor_render_app::application_update(const core_platform::bc_clock::update_param& p_clock_update_param, bool p_is_partial_update)
		{
		}

		void bc_editor_render_app::application_render(const core_platform::bc_clock::update_param& p_clock_update_param)
		{
		}

		bool bc_editor_render_app::application_event(core::bci_event& p_event)
		{
			auto* l_key_event = core::bci_message::as<platform::bc_app_event_key>(p_event);
			if (l_key_event)
			{
				/*if(l_key_event->get_key_state() == platform::bc_key_state::releasing && l_key_event->get_key() == platform::bc_key::kb_F)
				{
					m_game_system->get_render_system().get_render_pass<bc_cascaded_shadow_map_pass>()->capture_debug_shapes();
				}*/

				if (l_key_event->get_key_state() == platform::bc_key_state::pressing && l_key_event->get_key() == platform::bc_key::kb_space)
				{
					auto& l_input_system = m_game_system->get_input_system();
					auto& l_camera = *l_input_system.get_camera();
					auto* l_scene = m_game_system->get_scene();

					/*game::bc_actor l_actor;

					m_shape_throw_counter = m_shape_throw_counter % 4;
					switch (m_shape_throw_counter)
					{
					case 0:
						l_actor = l_entity_manager->create_entity("sphere");
						break;
					case 1:
						l_actor = l_entity_manager->create_entity("box");
						break;
					case 2:
						l_actor = l_entity_manager->create_entity("convex");
						break;
					case 3:
						l_actor = l_entity_manager->create_entity("capsule");
						break;
					}
					++m_shape_throw_counter;

					const auto l_position = l_camera.get_position();
					l_actor.add_event(game::bc_world_transform_actor_event(l_position));

					auto* l_rigid_component = l_actor.get_component<game::bc_rigid_body_component>();
					auto l_rigid = l_rigid_component->get_body();
					if (l_rigid.is_rigid_dynamic().is_valid())
					{
						const auto l_direction = l_camera.get_forward();
						l_rigid.set_linear_velocity(l_direction * 70);
					}

					l_scene->add_actor(l_actor);*/
					//l_scene->add_bullet(game::bc_bullet(l_camera.get_position(), l_camera.get_forward(), 250, 0.2f));
					//l_scene->create_actor("sample_rocket", bc_matrix4f_from_position_and_direction(l_camera.get_position(), l_camera.get_direction()));

					//const auto l_position1 = l_camera.get_position() + l_camera.get_forward() * 3;
					//const auto l_position2 = l_position1;// +core::bc_vector3f::right();
					//auto l_actor1 = l_scene->create_actor("capsule", core::bc_matrix4f::translation_matrix(l_position1));
					//auto l_actor2 = l_scene->create_actor("capsule", core::bc_matrix4f::translation_matrix(l_position2));
					//auto l_rigid_body1 = l_actor1.get_component<game::bc_rigid_dynamic_component>()->get_body();
					//auto l_rigid_body2 = l_actor2.get_component<game::bc_rigid_dynamic_component>()->get_body();

					//{
					//	physics::bc_scene_lock l_lock(&l_scene->get_px_scene());

					//	auto l_px_joint = m_game_system->get_physics_system().get_physics().create_d6_joint
					//	(
					//		&l_rigid_body1,
					//		physics::bc_transform(core::bc_vector3f(0.75, 0, 0)),
					//		&l_rigid_body2,
					//		physics::bc_transform(core::bc_vector3f(-0.75, 0, 0))
					//	);
					//	//l_px_joint->set_visualization(true);
					//	//l_px_joint->enable_limit(physics::bc_joint_cone_limit(core::bc_to_radian(90), core::bc_to_radian(90)));
					//	l_px_joint->set_motion(physics::bc_d6_axis::around_x, physics::bc_d6_motion::limited);
					//	l_px_joint->set_motion(physics::bc_d6_axis::around_y, physics::bc_d6_motion::limited);
					//	l_px_joint->set_motion(physics::bc_d6_axis::around_z, physics::bc_d6_motion::limited);
					//	l_px_joint->set_twist_limit(physics::bc_joint_angular_limit(core::bc_to_radian(45), -core::bc_to_radian(45)));
					//	l_px_joint->set_swing_limit(physics::bc_joint_cone_limit(core::bc_to_radian(45), core::bc_to_radian(45)));
					//	l_px_joint.release();

					//	const auto l_direction = l_camera.get_forward();
					//	l_rigid_body1.set_linear_velocity(l_direction * 40);
				}
				
				return true;
			}

			auto* l_exit_event = core::bci_message::as<platform::bc_app_event_exit>(p_event);
			if (l_exit_event)
			{
				auto& l_global_config = bc_get_global_config();
				auto* l_camera = m_game_system->get_input_system().get_camera();
				if (!l_camera)
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

		void bc_editor_render_app::application_unload_content(core::bc_content_stream_manager& p_stream_manager)
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