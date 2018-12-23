// [04/29/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/SceneGraph/bcOctalTreeSceneGraphNode.h"

namespace black_cat
{
	namespace game
	{
		bc_game_system::bc_game_system()
			: m_input_system(),
			m_file_system(),
			m_physics_system(),
			m_script_system(),
			m_render_system(),
			m_console(m_script_system)
		{
		}

		bc_game_system::~bc_game_system()
		{
			if (m_initialized)
			{
				destroy();
			}
		}

		void bc_game_system::set_scene(core::bc_unique_ptr<bc_scene> p_scene)
		{
			m_scene = std::move(p_scene);
		}

		void bc_game_system::render()
		{
			m_render_system.render(*m_scene);
		}

		void bc_game_system::update(core_platform::bc_clock::update_param p_clock_update_param)
		{
			bc_icamera::extend l_camera_extends;

			m_input_system.update(p_clock_update_param);

			m_input_system.get_camera().get_extend_points(l_camera_extends);
			auto l_render_system_update_params = bc_render_system::update_param
			(
				p_clock_update_param,
				m_input_system.get_camera().get_position(),
				m_input_system.get_camera().get_view(),
				m_input_system.get_camera().get_projection(),
				l_camera_extends
			);

			m_physics_system.update(p_clock_update_param);
			m_script_system.update(p_clock_update_param);
			m_render_system.update(l_render_system_update_params);
			m_console.update(p_clock_update_param);

			m_scene->update(m_physics_system, p_clock_update_param);
		}

		void bc_game_system::_initialize(bc_game_system_parameter p_parameter)
		{
			m_render_system.initialize(*core::bc_get_service<core::bc_content_stream_manager>(), std::move(p_parameter.m_render_system_parameter));
			m_physics_system.initialize();

			auto l_px_scene_builder = std::move(physics::bc_scene_builder()
				.enable_ccd());
			auto l_px_scene = m_physics_system.get_physics().create_scene(std::move(l_px_scene_builder));
			auto l_scene_graph = bc_scene_graph(std::move(p_parameter.m_scene_graph));

			m_scene = core::bc_make_unique<bc_scene>(std::move(l_scene_graph), std::move(l_px_scene));
		}

		void bc_game_system::_destroy()
		{
			m_scene.reset();
			m_physics_system.destroy();
			m_render_system.destroy();
		}
	}
}