// [02/08/2019 MRB]

#include <utility>

#include "Editor/EditorPCH.h"
#include "Editor/UICommand/bcSceneUICommand.h"
#include "Editor/UI/bcFormMainMenu.h"

namespace black_cat
{
	namespace editor
	{
		bc_scene_ui_command::bc_scene_ui_command(command_type p_type, core::bc_estring p_scene_path)
			: m_type(p_type),
			m_scene_path(std::move(p_scene_path)),
			m_scene(nullptr)
		{
		}

		bc_scene_ui_command::bc_scene_ui_command(command_type p_type, game::bc_scene* p_scene)
			: m_type(p_type),
			m_scene_path(),
			m_scene(p_scene)
		{
		}

		bc_scene_ui_command::bc_scene_ui_command(command_type p_type, core::bc_estring p_scene_path, game::bc_scene* p_scene)
			: m_type(p_type),
			m_scene_path(std::move(p_scene_path)),
			m_scene(p_scene)
		{
		}

		core::bc_string bc_scene_ui_command::title() const
		{
			return "SceneCommand";
		}

		bci_ui_command::state_ptr bc_scene_ui_command::create_state(state_context& p_context) const
		{
			return nullptr;
		}

		bool bc_scene_ui_command::update(update_context& p_context)
		{
			switch (m_type)
			{
				case bc_scene_ui_command_type::get_scene:
				{
					p_context.m_result.set_value(p_context.m_game_system.get_scene());
					break;
				}
				case bc_scene_ui_command_type::new_scene:
				{
					break;
				}
				case bc_scene_ui_command_type::load_scene:
				{
					auto l_scene = p_context.m_game_system.get_file_system()
						.get_content_manager()
						.load<game::bc_scene>(m_scene_path.c_str(), {}, core::bc_content_loader_parameter());
					p_context.m_game_system.set_scene(std::move(l_scene));
					break;
				}
				case bc_scene_ui_command_type::save_scene:
				{
					p_context.m_game_system.get_file_system()
						.get_content_manager()
						.save(*m_scene);
					break;
				}
				case bc_scene_ui_command_type::save_as_scene:
				{
					break;
				}
			}
			return false;
		}

		bc_scene_ui_command bc_scene_ui_command::for_get_scene()
		{
			return bc_scene_ui_command(command_type::get_scene, core::bc_estring());
		}

		bc_scene_ui_command bc_scene_ui_command::for_new_scene()
		{
			return bc_scene_ui_command(command_type::new_scene, core::bc_estring());
		}

		bc_scene_ui_command bc_scene_ui_command::for_load_scene(core::bc_estring p_path)
		{
			return bc_scene_ui_command(command_type::load_scene, std::move(p_path));
		}

		bc_scene_ui_command bc_scene_ui_command::for_save_scene(game::bc_scene* p_scene)
		{
			return bc_scene_ui_command(command_type::save_scene, p_scene);
		}

		bc_scene_ui_command bc_scene_ui_command::for_save_as_scene(game::bc_scene* p_scene, core::bc_estring p_path)
		{
			return bc_scene_ui_command(command_type::save_as_scene, std::move(p_path));
		}
	}
}