// [16/02/2017 MRB]

#pragma once

#include "Game/System/bcGameSystem.h"
#include "Editor/Application/bcUICommandService.h"
#include "Editor/Widget/bcWidgetD3DOutput.h"
#include "Editor/UI/bcFormTerrain.h"
#include "Editor/UI/bcFormObject.h"
#include "Editor/UI/bcFormObjectInsert.h"
#include "Editor/UI/bcFormDecal.h"
#include "Editor/UI/bcFormDecalInsert.h"
#include <QtWidgets/QtWidgets>
#include <QtWidgets/QDockWidget>

namespace black_cat
{
	namespace editor
	{
		enum class bc_form_tools_state
		{
			none,
			object_select,
			object_insert,
			terrain_height,
			terrain_smooth,
			terrain_material,
			terrain_material_smooth,
			decal_painter,
			decal_select
		};

		class bc_form_tools : public QObject
		{
			Q_OBJECT
			using state = bc_form_tools_state;

		public:
			bc_form_tools(game::bc_game_system& p_game_system,
				bc_ui_command_service& p_ui_command_service,
				bc_widget_d3d_output& p_render_widget,
				QDockWidget& p_container,
				QToolBox& p_tool_properties_container,
				bc_form_terrain& p_terrain_form,
				bc_form_object& p_object_form,
				bc_form_object_insert& p_object_insert_form,
				bc_form_decal& p_decal_form,
				bc_form_decal_insert& p_decal_insert_form);

		private slots:
			void objectSelectToggled(bool p_toggled);

			void objectInsertToggled(bool p_toggled);

			void terrainHeightToggled(bool p_toggled);

			void terrainSmoothToggled(bool p_toggled);

			void terrainMaterialToggled(bool p_toggled);

			void terrainMaterialSmoothToggled(bool p_toggled);
			
			void decalPainterToggled(bool p_toggled);
			
			void decalSelectToggled(bool p_toggled);

			void mousePressed(QMouseEvent* p_event);

			void mouseReleased(QMouseEvent* p_event);

			void mouseMoved(QMouseEvent* p_event);

		private:
			void _send_ui_command(const QMouseEvent* p_event);

			game::bc_game_system& m_game_system;
			bc_ui_command_service& m_ui_command_service;

			bc_widget_d3d_output& m_render_widget;
			bc_form_terrain& m_terrain_form;
			bc_form_object& m_object_form;
			bc_form_object_insert& m_object_insert_form;
			bc_form_decal& m_decal_form;
			bc_form_decal_insert& m_decal_insert_form;

			QDockWidget& m_tool_bar;
			QToolBox& m_tool_properties_container;
			QAbstractButton* m_object_select_button;
			QAbstractButton* m_object_insert_button;
			QAbstractButton* m_terrain_height_button;
			QAbstractButton* m_terrain_smooth_button;
			QAbstractButton* m_terrain_material_button;
			QAbstractButton* m_terrain_material_smooth_button;
			QAbstractButton* m_decal_painter_button;
			QAbstractButton* m_decal_select_button;

			state m_state;
			bool m_is_mouse_pressed;
		};
	}
}