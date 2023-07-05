// [12/11/2016 MRB]

#pragma once

#include "Editor/Application/bcEditorGameConsole.h"
#include "Editor/Application/bcRenderApplicationD3DWidgetOutputWindow.h"
#include "Editor/Application/bcEditorRenderApplicationThread.h"
#include "Editor/Widget/bcWidgetD3DOutput.h"
#include "Editor/Widget/bcWidgetConsole.h"
#include "Editor/QtAwesome/QtAwesome.h"
#include "Editor/UI/bcFormMainMenu.h"
#include "Editor/UI/bcFormTools.h"
#include "Editor/UI/bcFormMainToolBar.h"
#include "Editor/UI/bcFormTerrain.h"
#include "Editor/UI/bcFormObject.h"
#include "Editor/UI/bcFormObjectInsert.h"
#include "Editor/UI/bcFormDecalInsert.h"
#include "Editor/UI/bcFormDecal.h"
#include "Editor/GeneratedFiles/ui_bcBlackCatEditor.h"
#include <QtWidgets/QToolBox>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QAbstractButton>
#include <QtConcurrent/QtConcurrent>

namespace black_cat
{
	namespace editor
	{
		class bc_editor_app : public QMainWindow
		{
			Q_OBJECT

		public:
			bc_editor_app(HINSTANCE p_instance, const bcCHAR* p_cmd_line, bc_editor_render_app_factory p_render_app_factory);

			bc_editor_app(bc_editor_app&&) = delete;

			~bc_editor_app() override;

			bc_editor_app& operator=(bc_editor_app&&) = delete;

		protected:
			void focusInEvent(QFocusEvent* p_event) override;

			void focusOutEvent(QFocusEvent* p_event) override;

			void changeEvent(QEvent* p_event) override;

			void closeEvent(QCloseEvent* p_event) override;

		private slots:
			void scriptExecuted(const QString& p_string);

			void timerTimeout();

		private:
			void _load_style() const;

			void _load_icons() const;

			void _load_icon(QWidget* p_parent, QVariantMap& p_options) const;
			
			void _load_icon_button(QAbstractButton* p_bottom, QVariantMap& p_options) const;

			void _load_icon_toolbox(QToolBox* p_tool_box, QVariantMap& p_options) const;

			void _load_icon_toolbar(QToolBar* p_tool_bar, QVariantMap& p_options) const;

			Ui::bcBlackCatEditorClass m_ui;
			std::unique_ptr<QtAwesome> m_awesome;

			bc_ui_command_service* m_ui_command_service;
			bc_widget_d3d_output* m_d3d_widget;
			bc_widget_console* m_console_widget;
			std::unique_ptr<bc_render_application_d3dwidget_output_window> m_d3d_output_window;
			std::unique_ptr<bc_editor_game_console> m_editor_game_console;

			std::unique_ptr<bc_form_main_menu> m_form_main_menu;
			std::unique_ptr<bc_form_main_tool_bar> m_form_main_tool_bar;
			std::unique_ptr<bc_form_tools> m_form_tools;
			std::unique_ptr<bc_form_object_insert> m_form_object_insert;
			std::unique_ptr<bc_form_object> m_form_object;
			std::unique_ptr<bc_form_terrain> m_form_terrain;
			std::unique_ptr<bc_form_decal_insert> m_form_decal_insert;
			std::unique_ptr<bc_form_decal> m_form_decal;
			std::unique_ptr<QTimer> m_timer;

			bc_editor_render_app_thread m_render_app_thread;
		};
	}
}
