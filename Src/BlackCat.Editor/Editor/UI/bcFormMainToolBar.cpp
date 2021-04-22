// [04/22/2021 MRB]

#include "Editor/EditorPCH.h"
#include "Editor/UICommand/bcUIEditorModeCommand.h"
#include "Editor/UI/bcFormMainToolBar.h"

namespace black_cat
{
	namespace editor
	{
		bc_form_main_tool_bar::bc_form_main_tool_bar(bc_ui_command_service& p_command_service, QToolBar& p_container)
			: m_command_service(&p_command_service)
		{
			auto* l_actions_group = new QActionGroup(this);
			auto l_actions = p_container.actions();
			for(auto* l_action : l_actions)
			{
				l_actions_group->addAction(l_action);
			}
			
			auto l_find_action = [&](const char* p_action_name)
			{
				for(auto* l_action : l_actions)
				{
					if(l_action->objectName() == p_action_name)
					{
						return l_action;
					}
				}
				return static_cast<QAction*>(nullptr);
			};
			
			m_game_mode_button = l_find_action("gameModeButton");
			m_editor_mode_button = l_find_action("editorModeButton");

			QObject::connect(m_game_mode_button, SIGNAL(triggered(bool)), this, SLOT(gameModeClick(bool)));
			QObject::connect(m_editor_mode_button, SIGNAL(triggered(bool)), this, SLOT(editorModeClick(bool)));
		}

		void bc_form_main_tool_bar::gameModeClick(bool)
		{
			m_command_service->queue_command(bc_ui_editor_mode_command(false));
		}

		void bc_form_main_tool_bar::editorModeClick(bool)
		{
			m_command_service->queue_command(bc_ui_editor_mode_command(true));
		}
	}
}