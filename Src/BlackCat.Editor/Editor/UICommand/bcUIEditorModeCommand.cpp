// [04/22/2021 MRB]

#include "Editor/EditorPCH.h"

#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/File/bcPath.h"
#include "Game/Object/Scene/bcSceneCheckPoint.h"
#include "Game/bcEvent.h"
#include "Editor/UICommand/bcUIEditorModeCommand.h"

namespace black_cat
{
	namespace editor
	{
		bc_ui_editor_mode_command::bc_ui_editor_mode_command(bool p_editor_mode) noexcept
			: m_editor_mode(p_editor_mode)
		{
		}

		bc_ui_editor_mode_command::bc_ui_editor_mode_command(const bc_ui_editor_mode_command&) noexcept = default;

		bc_ui_editor_mode_command::~bc_ui_editor_mode_command() = default;

		bc_ui_editor_mode_command& bc_ui_editor_mode_command::operator=(const bc_ui_editor_mode_command&) noexcept = default;

		core::bc_string bc_ui_editor_mode_command::title() const
		{
			return "EditorMode";
		}

		bool bc_ui_editor_mode_command::update(update_context& p_context)
		{
			auto* l_scene = p_context.m_game_system.get_scene();
			if(!l_scene)
			{
				return false;
			}

			auto& l_content_manager = *core::bc_get_service<core::bc_content_manager>();
			auto l_path = core::bc_path(l_scene->get_path());
			const auto l_new_file_name = l_path.get_filename_without_extension() + bcL(".editor_checkpoint") + l_path.get_file_extension();
			l_path.set_filename(l_new_file_name.c_str());
			
			if(!m_editor_mode)
			{
				game::bc_scene_check_point l_check_point(*l_scene);
				l_content_manager.save_as(l_check_point, l_path.get_string_frame().c_str(), nullptr);
			}
			else
			{
				core::bc_content_loader_parameter l_instance_parameters;
				l_instance_parameters.add_value("scene", l_scene);

				auto l_check_point = l_content_manager.load<game::bc_scene_check_point>
				(
					l_path.get_string_frame().c_str(),
					nullptr,
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