// [12/11/2016 MRB]

#include "Editor/EditorPCH.h"
#include "Editor/Application/bcEditorApplication.h"
#include "Editor/Application/bcEditorRenderApplication.h"
#include "Editor/Application/bcUICommandService.h"
#include <QtGui/QtEvents>
#include <QtCore/QTextStream>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFrame>

namespace black_cat::editor
{
	bc_editor_app::bc_editor_app(HINSTANCE p_instance, const bcCHAR* p_cmd_line, bc_editor_render_app_factory p_render_app_factory)
		: QMainWindow(Q_NULLPTR)
	{
		setAttribute(Qt::WA_QuitOnClose);
		m_ui.setupUi(this);
		m_ui.mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
			
		m_d3d_widget = new bc_widget_d3d_output(this, m_ui.centralWidget->findChild<QFrame*>("leftRenderFrame"));
		m_d3d_output_window = std::make_unique<bc_render_application_d3dwidget_output_window>(m_d3d_widget);

		m_render_app_thread.start(p_instance, p_cmd_line, std::move(p_render_app_factory), m_d3d_output_window.get());

		m_awesome = std::make_unique<QtAwesome>(this);
		m_awesome->initFontAwesome();
		m_console_widget = new bc_widget_console(m_awesome.get(), m_ui.centralWidget->findChild<QWidget*>("consoleTab"));
		_load_style();
		_load_icons();
			
		m_render_app_thread.wait_for_initialization();

		// Now that game is available initialize console
		auto& l_game_system = *core::bc_get_service<game::bc_game_system>();
		auto& l_game_console = l_game_system.get_console();
		m_editor_game_console = std::make_unique<bc_editor_game_console>(l_game_console, *m_console_widget);
		m_editor_game_console->connect_widget(m_console_widget);
		l_game_console.set_implementation(m_editor_game_console.get());
			
		m_ui_command_service = core::bc_get_service<bc_ui_command_service>();
		m_form_main_menu = std::make_unique<bc_form_main_menu>(*m_ui.mainMenuBar, *m_ui_command_service);
		m_form_main_tool_bar = std::make_unique<bc_form_main_tool_bar>(*m_ui_command_service, *m_ui.mainToolBar, *m_d3d_widget);
		m_form_object = std::make_unique<bc_form_object>(*m_ui.centralWidget, *m_ui_command_service);
		m_form_object_insert = std::make_unique<bc_form_object_insert>(*m_ui.centralWidget);
		m_form_terrain = std::make_unique<bc_form_terrain>(*m_ui.centralWidget);
		m_form_decal = std::make_unique<bc_form_decal>(*m_ui.centralWidget, *m_ui_command_service);
		m_form_decal_insert = std::make_unique<bc_form_decal_insert>(*m_ui.centralWidget);
		m_form_tools = std::make_unique<bc_form_tools>
		(
			l_game_system,
			*m_ui_command_service, 
			*m_d3d_widget, 
			*m_ui.toolsDock, 
			*m_ui.rightToolBox, 
			*m_form_terrain, 
			*m_form_object, 
			*m_form_object_insert, 
			*m_form_decal,
			*m_form_decal_insert
		);
		m_timer = std::make_unique<QTimer>();
		m_timer->start(static_cast<bcINT>(1000.f / 60.f));

		QObject::connect(m_editor_game_console.get(), SIGNAL(scriptExecuted(const QString&)), this, SLOT(scriptExecuted(const QString&)));
		QObject::connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(timerTimeout()));

		m_d3d_widget->window_initialized();
	}

	bc_editor_app::~bc_editor_app()
	{
		m_render_app_thread.wait();
	}

	void bc_editor_app::focusInEvent(QFocusEvent* p_event)
	{
		QMainWindow::focusInEvent(p_event);
		m_d3d_widget->focus_event(p_event);
	}

	void bc_editor_app::focusOutEvent(QFocusEvent* p_event)
	{
		QMainWindow::focusOutEvent(p_event);
		m_d3d_widget->focus_event(p_event);
	}

	void bc_editor_app::changeEvent(QEvent* p_event)
	{
		QMainWindow::changeEvent(p_event);

		if (p_event->type() == QEvent::WindowStateChange)
		{
			QWindowStateChangeEvent* l_window_state_event = static_cast<QWindowStateChangeEvent*>(p_event);
			m_d3d_widget->window_state_change_event(l_window_state_event, windowState());
		}

		if (p_event->type() == QEvent::ActivationChange)
		{
			if (isActiveWindow())
			{
				QFocusEvent l_focus_event(QEvent::FocusIn);
				focusInEvent(&l_focus_event);
			}
			else
			{
				QFocusEvent l_focus_event(QEvent::FocusOut);
				focusOutEvent(&l_focus_event);
			}
		}
	}

	void bc_editor_app::closeEvent(QCloseEvent* p_event)
	{
		if (m_render_app_thread.is_still_running())
		{
			m_d3d_widget->close_event(p_event);
		}
	}
		
	void bc_editor_app::_load_style() const
	{
		QFile l_style_file(":/qdarkstyle/style.qss");

		if (l_style_file.open(QFile::ReadOnly | QFile::Text))
		{
			QTextStream ts(&l_style_file);

			qApp->setStyleSheet(ts.readAll());
		}
	}

	void bc_editor_app::_load_icons() const
	{
		QVariantMap l_icon_defaults;
		const QColor l_icon_default_color(224, 224, 224);
		l_icon_defaults.insert("color", l_icon_default_color);
		l_icon_defaults.insert("color-selected", l_icon_default_color);
		l_icon_defaults.insert("color-active", l_icon_default_color);
			
		_load_icon(m_ui.centralWidget, l_icon_defaults);
		_load_icon(m_ui.toolsDock, l_icon_defaults);
		_load_icon(m_ui.mainToolBar, l_icon_defaults);
	}

	void bc_editor_app::_load_icon(QWidget* p_parent, QVariantMap& p_options) const
	{
		auto* l_tool_box = qobject_cast<QToolBox*>(p_parent);
		if (l_tool_box)
		{
			_load_icon_toolbox(l_tool_box, p_options);
		}

		auto* l_tool_bar = qobject_cast<QToolBar*>(p_parent);
		if (l_tool_bar)
		{
			_load_icon_toolbar(l_tool_bar, p_options);
		}
			
		auto* l_button = qobject_cast<QToolButton*>(p_parent);
		if (l_button)
		{
			_load_icon_button(l_button, p_options);
		}

		const auto& l_children = p_parent->children();
		for (auto* l_child_obj : l_children)
		{
			auto* l_child_widget = qobject_cast<QWidget*>(l_child_obj);
			if(!l_child_widget)
			{
				continue;
			}
				
			_load_icon(l_child_widget, p_options);
		}
	}

	void bc_editor_app::_load_icon_button(QAbstractButton* p_bottom, QVariantMap& p_options) const
	{
		const auto l_font_name_property = p_bottom->property("fontAwesome");
		if (!l_font_name_property.isValid())
		{
			return;
		}

		const auto l_icon = m_awesome->icon(l_font_name_property.toString(), p_options);
		p_bottom->setIcon(l_icon);
	}

	void bc_editor_app::_load_icon_toolbox(QToolBox* p_tool_box, QVariantMap& p_options) const
	{
		const auto l_font_name_property = p_tool_box->property("fontAwesome");
		if(!l_font_name_property.isValid())
		{
			return;
		}

		const auto l_font_name_array = l_font_name_property.toString();
		auto l_font_names = l_font_name_array.split(',');

		const auto l_name_count = l_font_names.count();
		const auto l_tab_count = p_tool_box->count();

		for(bcSIZE i = 0, end = std::min(l_name_count, l_tab_count); i < end; ++i)
		{
			auto l_icon = m_awesome->icon(l_font_names[i], p_options);
			p_tool_box->setItemIcon(i, l_icon);
		}
	}

	void bc_editor_app::_load_icon_toolbar(QToolBar* p_tool_bar, QVariantMap& p_options) const
	{
		auto l_actions = p_tool_bar->actions();
		for(auto* l_action : l_actions)
		{
			const auto l_font_name_property = l_action->property("fontAwesome");
			if (!l_font_name_property.isValid())
			{
				return;
			}

			const auto l_icon = m_awesome->icon(l_font_name_property.toString(), p_options);
			l_action->setIcon(l_icon);
		}
	}

	void bc_editor_app::scriptExecuted(const QString& p_string)
	{
		statusBar()->showMessage("Script executed: " + p_string, 3000);
	}

	void bc_editor_app::timerTimeout()
	{
		if (!m_render_app_thread.is_still_running())
		{
			const auto l_exit_code = m_render_app_thread.get_result_code();
			QMessageBox::information(this, "Engine exited", QString("Engine exited with code %1").arg(QString::number(l_exit_code)));

			close();
			return;
		}

		m_d3d_widget->update_ui();
		m_editor_game_console->update_ui();
			
		bci_ui_command::update_ui_context l_context(*m_form_object, *m_form_object_insert, *m_form_decal, *m_form_decal_insert, *m_form_main_menu);
		m_ui_command_service->update_ui(l_context);
	}
}
