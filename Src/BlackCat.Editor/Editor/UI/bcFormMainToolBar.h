// [04/22/2021 MRB]

#pragma once

#include "Editor/Application/bcUICommandService.h"
#include <Qt3DInput/QAction>
#include <QtWidgets/QToolBar>

namespace black_cat
{
	namespace editor
	{
		class bc_form_main_tool_bar : public QObject
		{
			Q_OBJECT

		public:
			bc_form_main_tool_bar(bc_ui_command_service& p_command_service, QToolBar& p_container);

		private slots:
			void gameModeClick(bool);
			
			void editorModeClick(bool);
		
		private:
			bc_ui_command_service* m_command_service;
			QAction* m_game_mode_button;
			QAction* m_editor_mode_button;
			bool m_editor_mode;
		};
	}
}