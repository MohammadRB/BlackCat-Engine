// [04/29/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/bcGameSystem.h"

namespace black_cat
{
	namespace game
	{
		bc_game_system::bc_game_system()
			: m_input_system(),
			m_file_system(*core::bc_get_service<core::bc_content_manager>(), *core::bc_get_service<core::bc_content_stream_manager>()),
			m_physics_system(),
			m_script_system(),
			m_render_system(),
			m_console(),
			m_scene()
		{
		}

		bc_game_system::~bc_game_system()
		{
			if (m_initialized)
			{
				destroy();
			}
		}

		void bc_game_system::set_scene(bc_scene_ptr p_scene)
		{
			m_scene = std::move(p_scene);
		}

		void bc_game_system::render(const core_platform::bc_clock::update_param& p_clock_update_param)
		{
			if(m_scene)
			{
				m_render_system.render(bc_render_system::render_param(p_clock_update_param, m_input_system.get_camera(), *m_scene));
			}
		}

		void bc_game_system::update(const core_platform::bc_clock::update_param& p_clock_update_param)
		{
			m_input_system.update(p_clock_update_param);
			m_physics_system.update(p_clock_update_param);
			if(m_scene)
			{
				m_scene->update(m_physics_system, p_clock_update_param);
			}
			m_script_system.update(p_clock_update_param);
			m_console->update(p_clock_update_param);
			m_render_system.update(bc_render_system::update_param(p_clock_update_param, m_input_system.get_camera()));
		}

		void bc_game_system::_initialize(bc_game_system_parameter p_parameter)
		{
			m_physics_system.initialize();
			m_script_system.initialize(true);
			m_render_system.initialize(*core::bc_get_service<core::bc_content_stream_manager>(), std::move(p_parameter.m_render_system_parameter));
			m_console = core::bc_make_unique<bc_game_console>(core::bc_alloc_type::program, m_script_system);
		}

		void bc_game_system::_destroy()
		{
			m_scene.reset();
			m_console.reset();
			m_render_system.destroy();
			m_script_system.destroy();
			m_physics_system.destroy();
		}
	}
}