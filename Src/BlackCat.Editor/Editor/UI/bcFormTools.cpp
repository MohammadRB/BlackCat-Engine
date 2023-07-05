// [17/02/2017 MRB]

#include "Editor/EditorPCH.h"

#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/bcGameSystem.h"
#include "App/RenderPass/GBuffer/bcGBufferDecalPass.h"
#include "App/RenderPass/bcIconDrawPass.h"
#include "Editor/UI/bcFormTools.h"
#include "Editor/UICommand/bcTerrainHeightUICommand.h"
#include "Editor/UICommand/bcTerrainSmoothUICommand.h"
#include "Editor/UICommand/bcTerrainMaterialUICommand.h"
#include "Editor/UICommand/bcTerrainMaterialSmoothUICommand.h"
#include "Editor/UICommand/bcObjectSelectUICommand.h"
#include "Editor/UICommand/bcEntityNamesUICommand.h"
#include "Editor/UICommand/bcDecalNamesUICommand.h"
#include "Editor/UICommand/bcObjectInsertUICommand.h"
#include "Editor/UICommand/bcDecalPaintUICommand.h"
#include "Editor/UICommand/bcDecalSelectUICommand.h"
#include "Editor/UI/bcFormObjectInsert.h"

namespace black_cat::editor
{
	bc_form_tools::bc_form_tools(game::bc_game_system& p_game_system,
	                             bc_ui_command_service& p_ui_command_service,
	                             bc_widget_d3d_output& p_render_widget,
	                             QDockWidget& p_container,
	                             QToolBox& p_tool_properties_container,
	                             bc_form_terrain& p_terrain_form,
	                             bc_form_object& p_object_form,
	                             bc_form_object_insert& p_object_insert_form,
	                             bc_form_decal& p_decal_form,
	                             bc_form_decal_insert& p_decal_insert_form)
		: m_game_system(p_game_system),
		  m_ui_command_service(p_ui_command_service),
		  m_render_widget(p_render_widget),
		  m_terrain_form(p_terrain_form),
		  m_object_form(p_object_form),
		  m_object_insert_form(p_object_insert_form),
		  m_decal_form(p_decal_form),
		  m_decal_insert_form(p_decal_insert_form),
		  m_tool_bar(p_container),
		  m_tool_properties_container(p_tool_properties_container),
		  m_state(state::none),
		  m_is_mouse_pressed(false)
	{
		m_tool_bar.setTitleBarWidget(new QWidget(&m_tool_bar));

		m_object_select_button = m_tool_bar.findChild<QAbstractButton*>("objectSelectButton");
		m_object_insert_button = m_tool_bar.findChild<QAbstractButton*>("objectInsertButton");
		m_terrain_height_button = m_tool_bar.findChild<QAbstractButton*>("terrainHeightButton");
		m_terrain_smooth_button = m_tool_bar.findChild<QAbstractButton*>("terrainSmoothButton");
		m_terrain_material_button = m_tool_bar.findChild<QAbstractButton*>("terrainMaterialButton");
		m_terrain_material_smooth_button = m_tool_bar.findChild<QAbstractButton*>("terrainMaterialSmoothButton");
		m_decal_painter_button = m_tool_bar.findChild<QAbstractButton*>("decalPaintButton");
		m_decal_select_button = m_tool_bar.findChild<QAbstractButton*>("decalSelectButton");
			
		QObject::connect(m_object_select_button, SIGNAL(toggled(bool)), this, SLOT(objectSelectToggled(bool)));
		QObject::connect(m_object_insert_button, SIGNAL(toggled(bool)), this, SLOT(objectInsertToggled(bool)));
		QObject::connect(m_terrain_height_button, SIGNAL(toggled(bool)), this, SLOT(terrainHeightToggled(bool)));
		QObject::connect(m_terrain_smooth_button, SIGNAL(toggled(bool)), this, SLOT(terrainSmoothToggled(bool)));
		QObject::connect(m_terrain_material_button, SIGNAL(toggled(bool)), this, SLOT(terrainMaterialToggled(bool)));
		QObject::connect(m_terrain_material_smooth_button, SIGNAL(toggled(bool)), this, SLOT(terrainMaterialSmoothToggled(bool)));
		QObject::connect(m_decal_painter_button, SIGNAL(toggled(bool)), this, SLOT(decalPainterToggled(bool)));
		QObject::connect(m_decal_select_button, SIGNAL(toggled(bool)), this, SLOT(decalSelectToggled(bool)));
		QObject::connect(&m_render_widget, SIGNAL(mousePressed(QMouseEvent*)), this, SLOT(mousePressed(QMouseEvent*)));
		QObject::connect(&m_render_widget, SIGNAL(mouseReleased(QMouseEvent*)), this, SLOT(mouseReleased(QMouseEvent*)));
		QObject::connect(&m_render_widget, SIGNAL(mouseMoved(QMouseEvent*)), this, SLOT(mouseMoved(QMouseEvent*)));
	}

	void bc_form_tools::objectSelectToggled(bool p_toggled)
	{
		if(!p_toggled)
		{
			m_state = state::none;
			m_object_form.setSelectedActor(game::bc_actor(), core::bc_matrix4f::identity());
			return;
		}

		m_state = state::object_select;
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
		m_ui_command_service.queue_command(bc_entity_names_ui_command());
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

	void bc_form_tools::decalPainterToggled(bool p_toggled)
	{
		if (!p_toggled)
		{
			m_state = state::none;
			return;
		}

		m_state = state::decal_painter;
		m_tool_properties_container.setCurrentIndex(3);
		m_ui_command_service.queue_command(bc_decal_names_ui_command());
	}

	void bc_form_tools::decalSelectToggled(bool p_toggled)
	{
		if (!p_toggled)
		{	
			m_state = state::none;
			m_decal_form.setSelectedDecal(nullptr);
		}
		else
		{
			m_state = state::decal_select;
			m_tool_properties_container.setCurrentIndex(4);
		}

		/*auto* l_decal_render_pass = m_game_system.get_render_system().get_render_pass<bc_gbuffer_decal_pass>();
			if (l_decal_render_pass)
			{
				l_decal_render_pass->draw_decal_bounds(p_toggled);
			}*/
		auto* l_icon_render_pass = m_game_system.get_render_system().get_render_pass<bc_icon_draw_pass>();
		if (l_icon_render_pass)
		{
			l_icon_render_pass->draw_decal_icons(p_toggled);
		}
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
		if (m_is_mouse_pressed)
		{
			if (m_state != state::object_select &&
				m_state != state::object_insert &&
				m_state != state::decal_select &&
				m_state != state::decal_painter)
			{
				_send_ui_command(p_event);
			}
		}
		else if(m_state == state::object_select)
		{
			bc_object_select_ui_command l_command
			(
				m_render_widget.width(),
				m_render_widget.height(),
				p_event->x(),
				p_event->y(),
				true
			);
			m_ui_command_service.queue_command(std::move(l_command));
		}
		else if(m_state == state::decal_select)
		{
			bc_decal_select_ui_command l_command
			(
				m_render_widget.width(),
				m_render_widget.height(),
				p_event->x(),
				p_event->y(),
				true
			);
			m_ui_command_service.queue_command(std::move(l_command));
		}
	}

	void bc_form_tools::_send_ui_command(const QMouseEvent* p_event)
	{
		switch (m_state)
		{
		case state::object_select:
		{
			bc_object_select_ui_command l_command
			(
				m_render_widget.width(),
				m_render_widget.height(),
				p_event->x(),
				p_event->y(),
				false
			);
			m_ui_command_service.queue_command(std::move(l_command));
			break;
		}
		case state::object_insert:
		{
			QString l_selected_entity = m_object_insert_form.get_selected_entity();
			if(l_selected_entity.size() == 0)
			{
				break;
			}

			bc_object_insert_ui_command l_command
			(
				std::move(l_selected_entity),
				m_render_widget.width(),
				m_render_widget.height(),
				p_event->x(),
				p_event->y()
			);
			m_ui_command_service.queue_command(std::move(l_command));
			break;
		}
		case state::terrain_height:
		{
			bc_terrain_height_ui_command l_command
			(
				m_render_widget.width(),
				m_render_widget.height(),
				p_event->x(),
				p_event->y(),
				m_terrain_form.get_radius(),
				m_terrain_form.get_height() / 10.f
			);
			m_ui_command_service.queue_command(std::move(l_command));
			break;
		}
		case state::terrain_smooth:
		{
			bc_terrain_smooth_ui_command l_command
			(
				m_render_widget.width(),
				m_render_widget.height(),
				p_event->x(),
				p_event->y(),
				m_terrain_form.get_radius(),
				m_terrain_form.get_smooth()
			);
			m_ui_command_service.queue_command(std::move(l_command));
			break;
		}
		case state::terrain_material:
		{
			bc_terrain_material_ui_command l_command
			(
				m_render_widget.width(),
				m_render_widget.height(),
				p_event->x(),
				p_event->y(),
				m_terrain_form.get_radius(),
				m_terrain_form.get_material()
			);
			m_ui_command_service.queue_command(std::move(l_command));
			break;
		}
		case bc_form_tools_state::terrain_material_smooth:
		{
			bc_terrain_material_smooth_ui_command l_command
			(
				m_render_widget.width(),
				m_render_widget.height(),
				p_event->x(),
				p_event->y(),
				m_terrain_form.get_radius(),
				m_terrain_form.get_smooth()
			);
			m_ui_command_service.queue_command(std::move(l_command));
			break;
		}
		case bc_form_tools_state::decal_painter:
		{
			QString l_selected_entity = m_decal_insert_form.get_selected_decal();
			if (l_selected_entity.size() == 0)
			{
				break;
			}
				
			bc_decal_paint_ui_command l_command
			(
				std::move(l_selected_entity),
				m_render_widget.width(),
				m_render_widget.height(),
				p_event->x(),
				p_event->y()
			);
			m_ui_command_service.queue_command(std::move(l_command));
			break;
		}
		case bc_form_tools_state::decal_select:
		{
			bc_decal_select_ui_command l_command
			(
				m_render_widget.width(),
				m_render_widget.height(),
				p_event->x(),
				p_event->y(),
				false
			);
			m_ui_command_service.queue_command(std::move(l_command));
			break;
		}
		case state::none:
		default:
			break;
		}
	}
}
