// [11/13/2016 MRB]

#include "Editor/EditorPCH.h"
#include "Platform/bcEvent.h"
#include "Game/System/Input/bcFreeCamera.h"
#include "BlackCat/RenderPass/bcInitializePass.h"
#include "BlackCat/RenderPass/bcTerrainPassDx11.h"
#include "BlackCat/RenderPass/bcBackBufferOutputPass.h"
#include "Editor/Application/bcEditorHeightMapLoaderDx11.h"
#include "Editor/Application/bcEditorRenderApplication.h"
#include "Editor/Application/bcUICommandService.h"

namespace black_cat
{
	namespace editor
	{
		bc_editor_render_app::bc_editor_render_app() = default;

		bc_editor_render_app::~bc_editor_render_app() = default;

		void bc_editor_render_app::application_start_engine_components(game::bc_engine_component_parameter& p_engine_components, core::bc_service_manager& p_service_manager)
		{
			auto* l_game_system = p_service_manager.get_service<game::bc_game_system>();
			auto* l_content_stream_manager = p_service_manager.get_service< core::bc_content_stream_manager >();

			p_service_manager.register_service(core::bc_make_service<bc_ui_command_service>(*l_content_stream_manager, *l_game_system));
			l_content_stream_manager->register_loader< game::bc_height_map, bc_editor_height_map_loader_dx11 >
			(
				core::bc_make_loader< bc_editor_height_map_loader_dx11 >()
			);
		}

		void bc_editor_render_app::application_initialize(const bcCHAR* p_commandline)
		{
			game::bc_render_system& l_render_system = m_game_system->get_render_system();

			bcFLOAT l_back_buffer_width = l_render_system.get_device().get_back_buffer_width();
			bcFLOAT l_back_buffer_height = l_render_system.get_device().get_back_buffer_height();
			m_game_system->get_input_system().register_camera(core::bc_make_unique< game::bc_free_camera >
			(
				l_back_buffer_width / l_back_buffer_height,
				1.2,
				0.3,
				3000
			));
			l_render_system.add_render_pass(0, core::bc_make_unique< bc_initialize_pass >());
			l_render_system.add_render_pass(1, core::bc_make_unique< bc_terrain_pass_dx11 >());
			l_render_system.add_render_pass(2, core::bc_make_unique< bc_back_buffer_output_pass >());

			m_game_system->get_input_system().get_camera().set_position_lookat(core::bc_vector3f(0, 500, -1024), core::bc_vector3f(0, 0, 0));

			m_shape_throw_key_handle = core::bc_get_service< core::bc_event_manager >()->register_event_listener
			(
				platform::bc_app_event_key::event_name(),
				[this, l_counter = 0](const core::bc_ievent& p_event) mutable
				{
					const platform::bc_app_event_key& l_key_event = static_cast< const platform::bc_app_event_key& >(p_event);

					if(l_key_event.get_key_state() == platform::bc_key_state::pressing && l_key_event.get_key() == platform::bc_key::kb_space)
					{
						auto* l_entity_manager = core::bc_get_service< game::bc_entity_manager >();
						auto* l_scene = m_game_system->get_scene();

						game::bc_actor l_actor;

						l_counter = l_counter % 3;
						switch (l_counter)
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
							l_actor = l_entity_manager->create_entity("train");
							break;
						}
						++l_counter;

						auto* l_rigid_component = l_actor.get_component<game::bc_rigid_body_component>();
						auto l_rigid = l_rigid_component->get_body();

						auto l_position = m_game_system->get_input_system().get_camera().get_position();
						l_rigid_component->get_body().set_global_pose(physics::bc_transform(l_position));

						if(l_rigid.is_rigid_dynamic().is_valid())
						{
							auto l_direction = m_game_system->get_input_system().get_camera().get_forward();

							l_rigid_component->get_body().update_mass_inertia(10);
							l_rigid_component->get_body().set_linear_velocity(l_direction * 50);
						}
						else
						{
							core::bc_matrix4f l_mat;
							l_mat.translate(l_position.x, l_position.y, l_position.z);

							l_actor.get_component<game::bc_mesh_component>()->set_world_pos(l_mat);
						}

						l_scene->add_object(l_actor);
					}

					return true;
				}
			);
		}

		void bc_editor_render_app::application_load_content(core::bc_content_stream_manager* p_stream_manager)
		{
			auto* l_entity_manager = core::bc_get_service< game::bc_entity_manager >();
			auto* l_ui_command_manager = core::bc_get_service< bc_ui_command_service >();
			auto* l_scene = m_game_system->get_scene();

			p_stream_manager->load_content_stream(core::bc_alloc_type::program, "main");
			l_ui_command_manager->load_content();

			auto l_terrain = l_entity_manager->create_entity("crysis_heightmap");
			auto l_m16 = l_entity_manager->create_entity("m16A2");
			auto l_ship = l_entity_manager->create_entity("ship");

			l_scene->add_object(l_terrain);
			l_scene->add_object(l_m16);
			l_scene->add_object(l_ship);
		}

		void bc_editor_render_app::application_update(core_platform::bc_clock::update_param p_clock_update_param)
		{
			m_service_manager->update(p_clock_update_param);
		}

		void bc_editor_render_app::application_render(core_platform::bc_clock::update_param p_clock_update_param)
		{
			m_game_system->render();
		}

		bool bc_editor_render_app::application_event(core::bc_ievent& p_event)
		{
			return false;
		}

		void bc_editor_render_app::application_unload_content(core::bc_content_stream_manager* p_stream_manager)
		{
			auto* l_ui_command_manager = core::bc_get_service< bc_ui_command_service >();

			m_game_system->set_scene(nullptr);
			l_ui_command_manager->unload_content();
			p_stream_manager->unload_content_stream("main");
		}

		void bc_editor_render_app::application_destroy()
		{
			m_shape_throw_key_handle.reset();
		}

		void bc_editor_render_app::application_close_engine_components()
		{
		}
	}
}