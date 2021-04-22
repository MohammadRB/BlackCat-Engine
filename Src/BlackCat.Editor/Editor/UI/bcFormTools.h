// [02/16/2017 MRB]

#pragma once

#include "Game/System/bcGameSystem.h"
#include "Editor/Application/bcUICommandService.h"
#include "Editor/Widget/bcWidgetD3DOutput.h"
#include "Editor/UI/bcFormTerrain.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QDockWidget>

namespace black_cat
{
	namespace editor
	{
		enum class bc_form_tools_state
		{
			none,
			object_selection,
			object_insert,
			terrain_height,
			terrain_smooth,
			terrain_material,
			terrain_material_smooth
		};

		class bc_form_tools : public QObject
		{
			Q_OBJECT
			using state = bc_form_tools_state;

		public:
			bc_form_tools(bc_ui_command_service& p_ui_command_service,
				bc_widget_d3d_output& p_render_widget,
				QDockWidget& p_container,
				QToolBox& p_tool_properties_container,
				bc_form_terrain& p_terrain_form,
				bc_form_object_insert& p_object_insert_form);

		private slots:
			void objectSelectionToggled(bool p_toggled);

			void objectInsertToggled(bool p_toggled);

			void terrainHeightToggled(bool p_toggled);

			void terrainSmoothToggled(bool p_toggled);

			void terrainMaterialToggled(bool p_toggled);

			void terrainMaterialSmoothToggled(bool p_toggled);

			void mousePressed(QMouseEvent* p_event);

			void mouseReleased(QMouseEvent* p_event);

			void mouseMoved(QMouseEvent* p_event);

		private:
			void _send_ui_command(QMouseEvent* p_event);

			bc_ui_command_service& m_ui_command_service;

			bc_widget_d3d_output& m_render_widget;
			bc_form_terrain& m_terrain_form;
			bc_form_object_insert& m_object_insert_form;

			QDockWidget& m_tool_bar;
			QToolBox& m_tool_properties_container;
			QAbstractButton* m_object_selection;
			QAbstractButton* m_object_insert;
			QAbstractButton* m_terrain_height;
			QAbstractButton* m_terrain_smooth;
			QAbstractButton* m_terrain_material;
			QAbstractButton* m_terrain_material_smooth;

			state m_state;
			bool m_is_mouse_pressed;
		};
	}
}