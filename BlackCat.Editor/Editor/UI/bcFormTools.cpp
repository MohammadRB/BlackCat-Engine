// [02/17/2017 MRB]

#include "Editor/EditorPCH.h"
#include "Editor/UI/bcFormTools.h"
#include "Editor/UICommand/bcUITerrainHeightCommand.h"
#include "Editor/UICommand/bcUITerrainSmoothCommand.h"

namespace black_cat
{
	namespace editor
	{
		bc_form_tools::bc_form_tools(bc_ui_command_service& p_ui_command_service, 
			QDockWidget& p_container, 
			bc_d3d_output_widget& p_render_widget, 
			bc_form_terrain& p_terrain_form)
			: m_ui_command_service(p_ui_command_service),
			m_render_widget(p_render_widget),
			m_terrain_form(p_terrain_form),
			m_tool_bar(p_container),
			m_is_mouse_pressed(false)
		{
			m_tool_bar.setTitleBarWidget(new QWidget(&m_tool_bar));

			m_object_selection = m_tool_bar.findChild< QAbstractButton* >("objectSelectButton");
			m_terrain_height = m_tool_bar.findChild< QAbstractButton* >("terrainHeightButton");
			m_terrain_smooth = m_tool_bar.findChild< QAbstractButton* >("terrainSmoothButton");

			QObject::connect(m_object_selection, SIGNAL(toggled(bool)), this, SLOT(onObjectSelectionToggle(bool)));
			QObject::connect(m_terrain_height, SIGNAL(toggled(bool)), this, SLOT(onTerrainHeightToggle(bool)));
			QObject::connect(m_terrain_smooth, SIGNAL(toggled(bool)), this, SLOT(onTerrainSmoothToggle(bool)));

			QObject::connect(&m_render_widget, SIGNAL(mousePressed(QMouseEvent*)), this, SLOT(onMousePress(QMouseEvent*)));
			QObject::connect(&m_render_widget, SIGNAL(mouseReleased(QMouseEvent*)), this, SLOT(onMouseRelease(QMouseEvent*)));
			QObject::connect(&m_render_widget, SIGNAL(mouseMoved(QMouseEvent*)), this, SLOT(onMouseMove(QMouseEvent*)));
		}

		void bc_form_tools::onObjectSelectionToggle(bool p_toggled)
		{
			if(!p_toggled)
			{
				m_state = state::none;
				return;
			}

			m_state = state::object_selection;
		}

		void bc_form_tools::onTerrainHeightToggle(bool p_toggled)
		{
			if (!p_toggled)
			{
				m_state = state::none;
				return;
			}

			m_state = state::terrain_height;
		}

		void bc_form_tools::onTerrainSmoothToggle(bool p_toggled)
		{
			if (!p_toggled)
			{
				m_state = state::none;
				return;
			}

			m_state = state::terrain_smooth;
		}

		void bc_form_tools::onMousePress(QMouseEvent* p_event)
		{
			if(p_event->button() != Qt::MouseButton::LeftButton)
			{
				return;
			}

			m_is_mouse_pressed = true;
			_send_ui_command(p_event);
		}

		void bc_form_tools::onMouseRelease(QMouseEvent* p_event)
		{
			m_is_mouse_pressed = false;
		}

		void bc_form_tools::onMouseMove(QMouseEvent* p_event)
		{
			if(m_is_mouse_pressed)
			{
				_send_ui_command(p_event);
			}
		}

		void bc_form_tools::_send_ui_command(QMouseEvent* p_event)
		{
			switch (m_state)
			{
			case state::object_selection:
				break;
			case state::terrain_height:
				{
					bc_ui_terrain_height_command m_command
					(
						m_render_widget.width(),
						m_render_widget.height(),
						p_event->x(),
						p_event->y(),
						m_terrain_form.get_radius(),
						m_terrain_form.get_height()
					);
					m_ui_command_service.execute_command(std::move(m_command));
				}
				break;
			case state::terrain_smooth:
				{
					bc_ui_terrain_smooth_command m_command
					(
						m_render_widget.width(),
						m_render_widget.height(),
						p_event->x(),
						p_event->y(),
						m_terrain_form.get_radius(),
						m_terrain_form.get_smooth()
					);
					m_ui_command_service.execute_command(std::move(m_command));
				}
				break;
			case state::none:
				break;
			}
		}
	}
}
