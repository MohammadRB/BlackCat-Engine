// [04/22/2021 MRB]

#include "Editor/EditorPCH.h"

#include "Core/Messaging/Event/bcEventManager.h"
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
			game::bc_event_editor_mode l_event(m_editor_mode);
			core::bc_get_service<core::bc_event_manager>()->process_event(l_event);
			
			return false;
		}
	}	
}