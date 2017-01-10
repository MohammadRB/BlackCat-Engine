// [11/13/2016 MRB]

#include "Editor/EditorPCH.h"
#include "Game/System/Input/bcFreeCamera.h"
#include "BlackCat/RenderPass/bcInitializePass.h"
#include "BlackCat/RenderPass/bcTerrainPassDx11.h"
#include "BlackCat/RenderPass/bcBackBufferOutputPass.h"
#include "BlackCat/Loader/bcHeightMapLoaderDx11.h"
#include "Editor/Application/bcEditorRenderApplication.h"

namespace black_cat
{
	namespace editor
	{
		bc_editor_render_app::bc_editor_render_app() = default;

		bc_editor_render_app::~bc_editor_render_app() = default;

		void bc_editor_render_app::application_start_engine_components(game::bc_engine_component_parameter& p_engine_components)
		{
			auto* l_content_stream_manager = core::bc_get_service< core::bc_content_stream_manager >();

			l_content_stream_manager->register_loader< game::bc_height_map, bc_height_map_loader_dx11 >
			(
				core::bc_make_loader< bc_height_map_loader_dx11 >()
			);
		}

		void bc_editor_render_app::application_initialize(const bcCHAR* p_commandline)
		{
			bcFLOAT l_back_buffer_width = m_game_system->get_render_system().get_device().get_back_buffer_width();
			bcFLOAT l_back_buffer_height = m_game_system->get_render_system().get_device().get_back_buffer_height();
			m_game_system->get_input_system().register_camera(core::bc_make_unique< game::bc_free_camera >
			(
				l_back_buffer_width / l_back_buffer_height,
				1.2,
				0.3,
				3000
			));
			m_game_system->get_render_system().add_render_pass(0, core::bc_make_unique< bc_initialize_pass >());
			m_game_system->get_render_system().add_render_pass(1, core::bc_make_unique< bc_terrain_pass_dx11 >());
			m_game_system->get_render_system().add_render_pass(2, core::bc_make_unique< bc_back_buffer_output_pass >());

			m_game_system->get_input_system().get_camera().set_position_lookat(core::bc_vector3f(0, 100, -512), core::bc_vector3f(0, 0, 0));
		}

		void bc_editor_render_app::application_load_content(core::bc_content_stream_manager* p_stream_manager)
		{
			auto* l_entity_manager = core::bc_get_service< game::bc_entity_manager >();
			auto& l_scence_graph = m_game_system->get_render_system().get_scene_graph();

			p_stream_manager->load_content_stream(core::bc_alloc_type::program, "main");
			
			auto l_actor1 = l_entity_manager->create_entity("crysis_heightmap");
			auto l_actor2 = l_entity_manager->create_entity("m16A2");
			auto l_actor3 = l_entity_manager->create_entity("ship");

			l_scence_graph.add_object(l_actor1);
			l_scence_graph.add_object(l_actor2);
			l_scence_graph.add_object(l_actor3);
		}

		void bc_editor_render_app::application_update(core_platform::bc_clock::update_param p_clock_update_param)
		{
			m_service_manager->update(p_clock_update_param);

			m_game_system->get_render_system().render();
		}

		bool bc_editor_render_app::application_event(core::bc_ievent& p_event)
		{
			return false;
		}

		void bc_editor_render_app::application_unload_content(core::bc_content_stream_manager* p_stream_manager)
		{
			auto& l_scence_graph = m_game_system->get_render_system().get_scene_graph();

			l_scence_graph.clear();

			p_stream_manager->unload_content_stream("main");
		}

		void bc_editor_render_app::application_destroy()
		{
		}

		void bc_editor_render_app::application_close_engine_components()
		{
		}
	}
}
