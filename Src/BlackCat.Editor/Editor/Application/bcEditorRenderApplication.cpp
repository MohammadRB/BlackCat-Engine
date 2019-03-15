// [11/13/2016 MRB]

#include "Editor/EditorPCH.h"
#include "Platform/bcEvent.h"
#include "PhysicsImp/Body/bcRigidDynamic.h"
#include "Game/System/Input/bcFreeCamera.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"
#include "Game/Object/Scene/bcActor.hpp"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "BlackCat/RenderPass/bcInitializePass.h"
#include "BlackCat/RenderPass/bcTerrainPassDx11.h"
#include "BlackCat/RenderPass/bcMeshDrawPass.h"
#include "BlackCat/RenderPass/bcShapeDrawPass.h"
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
			auto* l_content_stream_manager = m_service_manager->get_service< core::bc_content_stream_manager >();

			m_service_manager->register_service(core::bc_make_service<bc_ui_command_service>(*l_content_stream_manager, *m_game_system));
			l_content_stream_manager->register_loader< game::bc_height_map, bc_editor_height_map_loader_dx11 >
			(
				core::bc_make_loader< bc_editor_height_map_loader_dx11 >()
			);
		}

		void bc_editor_render_app::application_initialize(game::bc_engine_application_parameter& p_parameters)
		{
			game::bc_render_system& l_render_system = m_game_system->get_render_system();

			m_game_system->get_input_system().register_camera(core::bc_make_unique< game::bc_free_camera >
			(
				l_render_system.get_device().get_back_buffer_width(),
				l_render_system.get_device().get_back_buffer_height(),
				1.2,
				0.3,
				3000
			));
			m_game_system->get_input_system().get_camera().set_look_at(core::bc_vector3f(0, 400, -1000), core::bc_vector3f(0, 0, 0));

			l_render_system.add_render_pass(0, bc_initialize_pass());
			l_render_system.add_render_pass(1, bc_terrain_pass_dx11());
			l_render_system.add_render_pass(2, bc_mesh_draw_pass());
			l_render_system.add_render_pass(3, bc_shape_draw_pass());

			m_shape_throw_key_handle = core::bc_get_service< core::bc_event_manager >()->register_event_listener<platform::bc_app_event_key>
			(
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

						const auto l_position = m_game_system->get_input_system().get_camera().get_position();
						l_actor.get_component<game::bc_mediate_component>()->set_world_position(l_position);

						auto* l_rigid_component = l_actor.get_component<game::bc_rigid_body_component>();
						auto l_rigid = l_rigid_component->get_body();
						if(l_rigid.is_rigid_dynamic().is_valid())
						{
							const auto l_direction = m_game_system->get_input_system().get_camera().get_forward();
							
							l_rigid.update_mass_inertia(10);
							l_rigid.set_linear_velocity(l_direction * 70);
						}

						l_scene->add_actor(l_actor);
					}

					return true;
				}
			);
		}

		void bc_editor_render_app::application_load_content(core::bc_content_stream_manager* p_stream_manager)
		{
			core::bc_get_service< bc_ui_command_service >()->load_content();

			const auto l_crysis_scene = core::bc_get_service< core::bc_content_manager >()->load< game::bc_scene >
			(
				m_game_system->get_file_system().get_content_path(bcL("Scene\\CrysisHeightMap.json")).c_str(),
				core::bc_content_loader_parameter()
			);
			m_game_system->set_scene(l_crysis_scene);
		}

		void bc_editor_render_app::application_update(core_platform::bc_clock::update_param p_clock_update_param)
		{
		}

		void bc_editor_render_app::application_render(core_platform::bc_clock::update_param p_clock_update_param)
		{
			m_game_system->render(p_clock_update_param);
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
