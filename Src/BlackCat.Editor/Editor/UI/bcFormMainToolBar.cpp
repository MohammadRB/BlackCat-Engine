// [04/22/2021 MRB]

#include "Editor/EditorPCH.h"

#include "Core/Messaging/Event/bcEventManager.h"
#include "Game/bcEvent.h"
#include "Editor/UICommand/bcEditorModeUICommand.h"
#include "Editor/UI/bcFormMainToolBar.h"
#include <QtWidgets/QtWidgets>

namespace black_cat
{
	namespace editor
	{
		bc_form_main_tool_bar::bc_form_main_tool_bar(bc_ui_command_service& p_command_service, const QToolBar& p_container, bc_widget_d3d_output& p_render_widget)
			: m_command_service(&p_command_service),
			m_render_widget(p_render_widget),
			m_editor_mode(true)
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
			m_pause_mode_button = l_find_action("pauseModeButton");
			m_editor_mode_button = l_find_action("editorModeButton");

			m_pause_mode_button->setEnabled(false);

			QObject::connect(m_game_mode_button, SIGNAL(triggered(bool)), this, SLOT(gameModeClick(bool)));
			QObject::connect(m_pause_mode_button, SIGNAL(triggered(bool)), this, SLOT(pauseModeClick(bool)));
			QObject::connect(m_editor_mode_button, SIGNAL(triggered(bool)), this, SLOT(editorModeClick(bool)));
		}

		void bc_form_main_tool_bar::gameModeClick(bool)
		{
			if(!m_editor_mode)
			{
				core::bc_get_service<core::bc_event_manager>()->queue_event
				(
					game::bc_event_game_pause_state(game::bc_event_game_pause_state::state::resumed), 
					0
				);
				return;
			}
			
			m_editor_mode = false;
			m_pause_mode_button->setEnabled(true);
			m_render_widget.set_editor_mode(false);
			m_command_service->queue_command(bc_editor_mode_ui_command(false));
		}

		void bc_form_main_tool_bar::pauseModeClick(bool)
		{
			core::bc_get_service<core::bc_event_manager>()->queue_event
			(
				game::bc_event_game_pause_state(game::bc_event_game_pause_state::state::paused), 
				0
			);
		}

		void bc_form_main_tool_bar::editorModeClick(bool)
		{
			if(m_editor_mode)
			{
				return;
			}
			
			m_editor_mode = true;
			m_pause_mode_button->setEnabled(false);
			m_render_widget.set_editor_mode(true);
			m_command_service->queue_command(bc_editor_mode_ui_command(true));
		}
	}
}