// [02/16/2017 MRB]

#pragma once

#include "Game/System/bcGameSystem.h"
#include "Editor/Application/bcUICommandService.h"
#include "Editor/Widget/bcD3DOutputWidget.h"
#include "Editor/UI/bcFormTerrain.h"

#include <QWidget>
#include <QDockWidget>

namespace black_cat
{
	namespace editor
	{
		class bc_form_tools : public QObject
		{
		private:
			Q_OBJECT;
			enum class state
			{
				none,
				object_selection,
				terrain_height,
				terrain_smooth
			};

		public:
			explicit bc_form_tools(bc_ui_command_service& p_ui_command_service, 
				QDockWidget& p_container, 
				bc_d3d_output_widget& p_render_widget, 
				bc_form_terrain& p_terrain_form);

		protected:

		private slots:
			void onObjectSelectionToggle(bool p_toggled);

			void onTerrainHeightToggle(bool p_toggled);

			void onTerrainSmoothToggle(bool p_toggled);

			void onMousePress(QMouseEvent* p_event);

			void onMouseRelease(QMouseEvent* p_event);

			void onMouseMove(QMouseEvent* p_event);

		private:
			void _send_ui_command(QMouseEvent* p_event);

			bc_ui_command_service& m_ui_command_service;

			bc_d3d_output_widget& m_render_widget;
			bc_form_terrain& m_terrain_form;

			QDockWidget& m_tool_bar;
			QAbstractButton* m_object_selection;
			QAbstractButton* m_terrain_height;
			QAbstractButton* m_terrain_smooth;

			state m_state;
			bool m_is_mouse_pressed;
		};
	}
}