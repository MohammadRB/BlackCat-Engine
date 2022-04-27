// [04/22/2021 MRB]

#include "Editor/EditorPCH.h"

#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/File/bcPath.h"
#include "Game/Object/Scene/bcSceneCheckPoint.h"
#include "Game/bcEvent.h"
#include "Editor/UICommand/bcEditorModeUICommand.h"

namespace black_cat
{
	namespace editor
	{
		bc_editor_mode_ui_command::bc_editor_mode_ui_command(bool p_editor_mode) noexcept
			: m_editor_mode(p_editor_mode)
		{
		}

		bc_editor_mode_ui_command::bc_editor_mode_ui_command(const bc_editor_mode_ui_command&) noexcept = default;

		bc_editor_mode_ui_command::~bc_editor_mode_ui_command() = default;

		bc_editor_mode_ui_command& bc_editor_mode_ui_command::operator=(const bc_editor_mode_ui_command&) noexcept = default;

		core::bc_string bc_editor_mode_ui_command::title() const
		{
			return "EditorMode";
		}

		bool bc_editor_mode_ui_command::update(update_context& p_context)
		{
			auto* l_scene = p_context.m_game_system.get_scene();
			if(!l_scene)
			{
				return false;
			}

			auto& l_content_manager = *core::bc_get_service<core::bc_content_manager>();
			const auto l_checkpoint_path = game::bc_scene_checkpoint::get_checkpoint_path(*l_scene, bcL("editor_checkpoint"));

			if(!m_editor_mode)
			{
				game::bc_scene_checkpoint l_check_point(*l_scene);
				l_content_manager.save_as(l_check_point, l_checkpoint_path.get_string_frame().c_str(), {});
			}
			else
			{
				core::bc_content_loader_parameter l_instance_parameters;
				l_instance_parameters.add_or_update("scene", core::bc_any(l_scene));

				auto l_check_point = l_content_manager.load<game::bc_scene_checkpoint>
				(
					l_checkpoint_path.get_string_frame().c_str(),
					{},
					core::bc_content_loader_parameter(),
					l_instance_parameters
				);
			}
			
			game::bc_event_editor_mode l_event(m_editor_mode);
			core::bc_get_service<core::bc_event_manager>()->process_event(l_event);
			
			return false;
		}
	}	
}