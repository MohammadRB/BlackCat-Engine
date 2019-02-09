// [02/17/2017 MRB]

#include "Editor/EditorPCH.h"
#include "Editor/UI/bcFormTools.h"
#include "Editor/UICommand/bcUITerrainHeightCommand.h"
#include "Editor/UICommand/bcUITerrainSmoothCommand.h"
#include "Editor/UICommand/bcUITerrainMaterialCommand.h"
#include "Editor/UICommand/bcUITerrainMaterialSmoothCommand.h"
#include "Editor/UICommand/bcUIObjectSelectCommand.h"
#include "Editor/UICommand/bcUIObjectListCommand.h"
#include "Editor/UICommand/bcUIObjectInsertCommand.h"
#include "Editor/UI/bcFormObjectInsert.h"

namespace black_cat
{
	namespace editor
	{
		bc_form_tools::bc_form_tools(bc_ui_command_service& p_ui_command_service,
			bc_d3d_output_widget& p_render_widget,
			QDockWidget& p_container,
			QToolBox& p_tool_properties_container,
			bc_form_terrain& p_terrain_form,
			bc_form_object_insert& p_object_insert_form)
			: m_ui_command_service(p_ui_command_service),
			m_render_widget(p_render_widget),
			m_terrain_form(p_terrain_form),
			m_object_insert_form(p_object_insert_form),
			m_tool_bar(p_container),
			m_tool_properties_container(p_tool_properties_container),
			m_state(state::none),
			m_is_mouse_pressed(false)
		{
			m_tool_bar.setTitleBarWidget(new QWidget(&m_tool_bar));

			m_object_selection = m_tool_bar.findChild< QAbstractButton* >("objectSelectButton");
			m_object_insert = m_tool_bar.findChild< QAbstractButton* >("objectInsertButton");
			m_terrain_height = m_tool_bar.findChild< QAbstractButton* >("terrainHeightButton");
			m_terrain_smooth = m_tool_bar.findChild< QAbstractButton* >("terrainSmoothButton");
			m_terrain_material = m_tool_bar.findChild< QAbstractButton* >("terrainMaterialButton");
			m_terrain_material_smooth = m_tool_bar.findChild< QAbstractButton* >("terrainMaterialSmoothButton");
			
			QObject::connect(m_object_selection, SIGNAL(toggled(bool)), this, SLOT(objectSelectionToggled(bool)));
			QObject::connect(m_object_insert, SIGNAL(toggled(bool)), this, SLOT(objectInsertToggled(bool)));
			QObject::connect(m_terrain_height, SIGNAL(toggled(bool)), this, SLOT(terrainHeightToggled(bool)));
			QObject::connect(m_terrain_smooth, SIGNAL(toggled(bool)), this, SLOT(terrainSmoothToggled(bool)));
			QObject::connect(m_terrain_material, SIGNAL(toggled(bool)), this, SLOT(terrainMaterialToggled(bool)));
			QObject::connect(m_terrain_material_smooth, SIGNAL(toggled(bool)), this, SLOT(terrainMaterialSmoothToggled(bool)));\
			QObject::connect(&m_render_widget, SIGNAL(mousePressed(QMouseEvent*)), this, SLOT(mousePressed(QMouseEvent*)));
			QObject::connect(&m_render_widget, SIGNAL(mouseReleased(QMouseEvent*)), this, SLOT(mouseReleased(QMouseEvent*)));
			QObject::connect(&m_render_widget, SIGNAL(mouseMoved(QMouseEvent*)), this, SLOT(mouseMoved(QMouseEvent*)));
		}

		void bc_form_tools::objectSelectionToggled(bool p_toggled)
		{
			if(!p_toggled)
			{
				m_state = state::none;
				return;
			}

			m_state = state::object_selection;
			m_tool_properties_container.setCurrentIndex(0);
		}

		void bc_form_tools::objectInsertToggled(bool p_toggled)
		{
			if (!p_toggled)
			{
				m_state = state::none;
				return;
			}

			m_state = state::object_insert;
			m_tool_properties_container.setCurrentIndex(1);
			m_ui_command_service.queue_command(bc_ui_object_list_command());
		}

		void bc_form_tools::terrainHeightToggled(bool p_toggled)
		{
			if (!p_toggled)
			{
				m_state = state::none;
				return;
			}

			m_state = state::terrain_height;
			m_tool_properties_container.setCurrentIndex(2);
		}

		void bc_form_tools::terrainSmoothToggled(bool p_toggled)
		{
			if (!p_toggled)
			{
				m_state = state::none;
				return;
			}

			m_state = state::terrain_smooth;
			m_tool_properties_container.setCurrentIndex(2);
		}

		void bc_form_tools::terrainMaterialToggled(bool p_toggled)
		{
			if (!p_toggled)
			{
				m_state = state::none;
				return;
			}

			m_state = state::terrain_material;
			m_tool_properties_container.setCurrentIndex(2);
		}

		void bc_form_tools::terrainMaterialSmoothToggled(bool p_toggled)
		{
			if (!p_toggled)
			{
				m_state = state::none;
				return;
			}

			m_state = state::terrain_material_smooth;
			m_tool_properties_container.setCurrentIndex(2);
		}

		void bc_form_tools::mousePressed(QMouseEvent* p_event)
		{
			if(p_event->button() != Qt::MouseButton::LeftButton)
			{
				return;
			}

			m_is_mouse_pressed = true;
			_send_ui_command(p_event);
		}

		void bc_form_tools::mouseReleased(QMouseEvent* p_event)
		{
			m_is_mouse_pressed = false;
		}

		void bc_form_tools::mouseMoved(QMouseEvent* p_event)
		{
			if (m_is_mouse_pressed &&
				m_state != state::object_selection &&
				m_state != state::object_insert)
			{
				_send_ui_command(p_event);
			}
		}

		void bc_form_tools::_send_ui_command(QMouseEvent* p_event)
		{
			switch (m_state)
			{
			case state::object_selection:
				{
					bc_ui_object_select_command l_command
					(
						m_render_widget.width(),
						m_render_widget.height(),
						p_event->x(),
						p_event->y()
					);
					m_ui_command_service.queue_command(std::move(l_command));
				}
				break;
			case state::object_insert:
				{
					QString l_selected_entity = m_object_insert_form.get_selected_entity();
					if(l_selected_entity.size() == 0)
					{
						break;
					}

					bc_ui_object_insert_command l_command
					(
						l_selected_entity,
						m_render_widget.width(),
						m_render_widget.height(),
						p_event->x(),
						p_event->y()
					);
					m_ui_command_service.queue_command(std::move(l_command));
				}
				break;
			case state::terrain_height:
				{
					bc_ui_terrain_height_command l_command
					(
						m_render_widget.width(),
						m_render_widget.height(),
						p_event->x(),
						p_event->y(),
						m_terrain_form.get_radius(),
						m_terrain_form.get_height()
					);
					m_ui_command_service.queue_command(std::move(l_command));
				}
				break;
			case state::terrain_smooth:
				{
					bc_ui_terrain_smooth_command l_command
					(
						m_render_widget.width(),
						m_render_widget.height(),
						p_event->x(),
						p_event->y(),
						m_terrain_form.get_radius(),
						m_terrain_form.get_smooth()
					);
					m_ui_command_service.queue_command(std::move(l_command));
				}
				break;
			case state::terrain_material:
				{
					bc_ui_terrain_material_command l_command
					(
						m_render_widget.width(),
						m_render_widget.height(),
						p_event->x(),
						p_event->y(),
						m_terrain_form.get_radius(),
						m_terrain_form.get_material()
					);
					m_ui_command_service.queue_command(std::move(l_command));
				}
				break;
			case bc_form_tools_state::terrain_material_smooth:
				{
					bc_ui_terrain_material_smooth_command l_command
					(
						m_render_widget.width(),
						m_render_widget.height(),
						p_event->x(),
						p_event->y(),
						m_terrain_form.get_radius(),
						m_terrain_form.get_smooth()
					);
					m_ui_command_service.queue_command(std::move(l_command));
				}
				break;
			case state::none:
			default:
				break;
			}
		}
	}
}
