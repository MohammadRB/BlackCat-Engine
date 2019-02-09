// [02/08/2019 MRB]

#include "Editor/EditorPCH.h"
#include "Editor/UICommand/bcUISceneCommand.h"
#include "Editor/UI/bcFormMainMenu.h"

namespace black_cat
{
	namespace editor
	{
		bc_ui_scene_command::bc_ui_scene_command(command_type p_type, core::bc_estring p_scene_path)
			: m_type(p_type),
			m_scene_path(std::move(p_scene_path)),
			m_scene(nullptr)
		{
		}

		bc_ui_scene_command::bc_ui_scene_command(command_type p_type, game::bc_scene* p_scene)
			: m_type(p_type),
			m_scene_path(),
			m_scene(p_scene)
		{
		}

		bc_ui_scene_command::bc_ui_scene_command(command_type p_type, core::bc_estring p_scene_path, game::bc_scene* p_scene)
			: m_type(p_type),
			m_scene_path(std::move(p_scene_path)),
			m_scene(p_scene)
		{
		}

		core::bc_string bc_ui_scene_command::title() const
		{
			return "SceneCommand";
		}

		bc_iui_command::state_ptr bc_ui_scene_command::create_state(state_context& p_context) const
		{
			return nullptr;
		}

		bool bc_ui_scene_command::update(update_context& p_context)
		{
			switch (m_type)
			{
			case bc_ui_scene_command_type::get_scene:
				m_scene = p_context.m_game_system.get_scene();
				break;
			case bc_ui_scene_command_type::new_scene:
				break;
			case bc_ui_scene_command_type::load_scene:
				break;
			case bc_ui_scene_command_type::save_scene:
				break;
			case bc_ui_scene_command_type::save_as_scene:
				break;
			default:;
			}
			return false;
		}

		void bc_ui_scene_command::update_ui(update_ui_context& p_context)
		{
			if(m_type == command_type::get_scene)
			{
				p_context.m_form_main_menu.set_scene(m_scene);
			}
		}

		bc_ui_scene_command bc_ui_scene_command::for_get_scene()
		{
			return bc_ui_scene_command(command_type::get_scene, core::bc_estring());
		}

		bc_ui_scene_command bc_ui_scene_command::for_new_scene()
		{
			return bc_ui_scene_command(command_type::new_scene, core::bc_estring());
		}

		bc_ui_scene_command bc_ui_scene_command::for_load_scene(core::bc_estring p_path)
		{
			return bc_ui_scene_command(command_type::load_scene, p_path);
		}

		bc_ui_scene_command bc_ui_scene_command::for_save_scene(game::bc_scene* p_scene)
		{
			return bc_ui_scene_command(command_type::save_scene, p_scene);
		}

		bc_ui_scene_command bc_ui_scene_command::for_save_as_scene(game::bc_scene* p_scene, core::bc_estring p_path)
		{
			return bc_ui_scene_command(command_type::save_as_scene, p_path);
		}
	}
}