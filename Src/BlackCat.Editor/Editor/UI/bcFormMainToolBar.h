// [22/04/2021 MRB]

#pragma once

#include "Editor/Application/bcUICommandService.h"
#include "Editor/Widget/bcWidgetD3DOutput.h"
#include <QtWidgets/QToolBar>

namespace black_cat
{
	namespace editor
	{
		class bc_form_main_tool_bar : public QObject
		{
			Q_OBJECT

		public:
			bc_form_main_tool_bar(bc_ui_command_service& p_command_service, const QToolBar& p_container, bc_widget_d3d_output& p_render_widget);

		private slots:
			void gameModeClick(bool);

			void pauseModeClick(bool);
			
			void editorModeClick(bool);
		
		private:
			bc_ui_command_service* m_command_service;
			bc_widget_d3d_output& m_render_widget;
			QAction* m_game_mode_button;
			QAction* m_pause_mode_button;
			QAction* m_editor_mode_button;
			bool m_editor_mode;
		};
	}
}