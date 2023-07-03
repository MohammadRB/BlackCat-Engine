// [13/11/2016 MRB]

#include "Editor/EditorPCH.h"

#include "Core/Messaging/Event/bcEventManager.h"
#include "Platform/bcEvent.h"
#include "Editor/Widget/bcWidgetD3DOutput.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QtWidgets>

namespace black_cat
{
	namespace editor
	{
		bc_widget_d3d_output::bc_widget_d3d_output(QWidget* p_parent)
			: QWidget(p_parent),
			m_editor_mode(true),
			m_last_mouse_x(0),
			m_last_mouse_y(0)
		{
			setAttribute(Qt::WA_PaintOnScreen, true);
			setAttribute(Qt::WA_NativeWindow, true);
			setMouseTracking(true);
			setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
			setFocusPolicy(Qt::ClickFocus);
			
			QHBoxLayout* l_horizontal_layout = new QHBoxLayout(p_parent);
			l_horizontal_layout->setMargin(0);
			l_horizontal_layout->setSpacing(0);
			l_horizontal_layout->addWidget(this);

			m_win_id = reinterpret_cast<HWND>(winId());
		}

		bc_widget_d3d_output::~bc_widget_d3d_output() = default;

		QPaintEngine* bc_widget_d3d_output::paintEngine() const
		{
			return nullptr;
		}

		graphic::bc_device_output bc_widget_d3d_output::get_device_output() const
		{
			return graphic::bc_device_output(graphic::bc_device_output::platform_pack(m_win_id));
		}

		void bc_widget_d3d_output::set_editor_mode(bool p_value) noexcept
		{
			m_editor_mode = p_value;
		}

		void bc_widget_d3d_output::mousePressEvent(QMouseEvent* p_event)
		{
			platform::bc_key l_key;

			switch (p_event->button())
			{
			case Qt::LeftButton: 
				l_key = platform::bc_key::ms_left_button;
				break;
			case Qt::RightButton: 
				l_key = platform::bc_key::ms_right_button;
				break;
			case Qt::MiddleButton: 
				l_key = platform::bc_key::ms_middle_button;
				break;
			default: 
				return;
			}

			platform::bc_app_event_key l_event(l_key, platform::bc_key_state::pressing);
			core::bc_get_service<core::bc_event_manager>()->queue_event(l_event, 0);

			emit mousePressed(p_event);
		}

		void bc_widget_d3d_output::mouseReleaseEvent(QMouseEvent* p_event)
		{
			platform::bc_key l_key;

			switch (p_event->button())
			{
			case Qt::LeftButton:
				l_key = platform::bc_key::ms_left_button;
				break;
			case Qt::RightButton:
				l_key = platform::bc_key::ms_right_button;
				break;
			case Qt::MiddleButton:
				l_key = platform::bc_key::ms_middle_button;
				break;
			default:
				return;
			}

			platform::bc_app_event_key l_event(l_key, platform::bc_key_state::releasing);
			core::bc_get_service<core::bc_event_manager>()->queue_event(l_event, 0);

			emit mouseReleased(p_event);
		}

		void bc_widget_d3d_output::mouseMoveEvent(QMouseEvent* p_event)
		{
			// Engine pointing device will raise pointing events regardless of existence of editor render widget which is responsible
			// for raising mouse and keyboard events, so in game mode we should not send events to prevent double pointing events.
			if(!m_editor_mode)
			{
				return;
			}

			platform::bc_pointing_device_state l_pointing_state;
			l_pointing_state.m_x = p_event->globalX();
			l_pointing_state.m_y = p_event->globalY();
			l_pointing_state.m_dx = p_event->globalX() - m_last_mouse_x;
			l_pointing_state.m_dy = p_event->globalY() - m_last_mouse_y;

			platform::bc_app_event_pointing l_event(l_pointing_state);
			core::bc_get_service<core::bc_event_manager>()->queue_event(l_event, 0);

			m_last_mouse_x = p_event->globalX();
			m_last_mouse_y = p_event->globalY();

			emit mouseMoved(p_event);
		}

		void bc_widget_d3d_output::keyPressEvent(QKeyEvent* p_event)
		{
			platform::bc_app_event_key l_event = platform::bc_app_event_key
			(
				static_cast<platform::bc_key>(p_event->nativeVirtualKey()),
				platform::bc_key_state::pressing
			);
			core::bc_get_service<core::bc_event_manager>()->queue_event(l_event, 0);
		}

		void bc_widget_d3d_output::keyReleaseEvent(QKeyEvent* p_event)
		{
			if(p_event->isAutoRepeat())
			{
				return;
			}

			platform::bc_app_event_key l_event
			(
				static_cast<platform::bc_key>(p_event->nativeVirtualKey()),
				platform::bc_key_state::releasing
			);
			core::bc_get_service<core::bc_event_manager>()->queue_event(l_event, 0);
		}

		void bc_widget_d3d_output::resizeEvent(QResizeEvent* p_resize)
		{
			platform::bc_app_event_window_resize l_event = platform::bc_app_event_window_resize
			(
				reinterpret_cast<platform::bc_window::id>(m_win_id),
				p_resize->size().width(),
				p_resize->size().height(),
				false
			);
			core::bc_get_service<core::bc_event_manager>()->queue_event(l_event, 0);
		}

		void bc_widget_d3d_output::focus_event(QFocusEvent* p_event)
		{
			platform::bc_app_event_window_focus l_event
			(
				reinterpret_cast<platform::bc_window::id>(m_win_id),
				p_event->gotFocus()
			);
			core::bc_get_service<core::bc_event_manager>()->queue_event(l_event, 0);
		}

		void bc_widget_d3d_output::window_state_change_event(QWindowStateChangeEvent* p_event, Qt::WindowStates p_window_state)
		{
			auto* l_window_state_event = static_cast<QWindowStateChangeEvent*>(p_event);
			platform::bc_app_event_window_state::state l_state = platform::bc_app_event_window_state::state::restored;

			if (!(l_window_state_event->oldState() & Qt::WindowMaximized) && (p_window_state & Qt::WindowMaximized))
			{
				l_state = platform::bc_app_event_window_state::state::maximized;
			}
			else if (!(l_window_state_event->oldState() & Qt::WindowMinimized) && (p_window_state & Qt::WindowMinimized))
			{
				l_state = platform::bc_app_event_window_state::state::minimized;
			}

			platform::bc_app_event_window_state l_event = platform::bc_app_event_window_state
			(
				reinterpret_cast<platform::bc_window::id>(m_win_id),
				l_state
			);
			core::bc_get_service<core::bc_event_manager>()->queue_event(l_event, 0);
		}

		void bc_widget_d3d_output::close_event(QCloseEvent* p_event)
		{
			platform::bc_app_event_window_close l_event(reinterpret_cast<platform::bc_window::id>(m_win_id));
			core::bc_get_service<core::bc_event_manager>()->queue_event(l_event, 0);
		}
	}
}