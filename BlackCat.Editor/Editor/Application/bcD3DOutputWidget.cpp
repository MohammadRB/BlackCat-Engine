// [11/13/2016 MRB]

#include "Editor/EditorPCH.h"
#include "Core/Event/bcEventManager.h"
#include "Platform/bcEvent.h"
#include "Editor/Application/bcD3DOutputWidget.h"

namespace black_cat
{
	namespace editor
	{
		bc_d3d_output_widget::bc_d3d_output_widget(QWidget* p_parent)
			: QWidget(p_parent),
			m_last_mouse_x(0),
			m_last_mouse_y(0)
		{
			setAttribute(Qt::WA_PaintOnScreen, true);
			setAttribute(Qt::WA_NativeWindow, true);
			setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
			setFocusPolicy(Qt::ClickFocus);

			QHBoxLayout* l_horizontal_layout = new QHBoxLayout(p_parent);
			l_horizontal_layout->setMargin(0);
			l_horizontal_layout->setSpacing(0);
			l_horizontal_layout->addWidget(this);

			m_win_id = reinterpret_cast<HWND>(winId());
		}

		bc_d3d_output_widget::~bc_d3d_output_widget() = default;

		QPaintEngine* bc_d3d_output_widget::paintEngine() const
		{
			return nullptr;
		}

		graphic::bc_device_output bc_d3d_output_widget::get_device_output() const
		{
			return graphic::bc_device_output(graphic::bc_device_output::parameter(m_win_id));
		}

		void bc_d3d_output_widget::mousePressEvent(QMouseEvent* p_event)
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
			
			platform::bc_app_event_key l_key_event(l_key, platform::bc_key_state::pressing);

			core::bc_get_service< core::bc_event_manager >()->queue_event(core::bc_make_event(l_key_event), 0);
		}

		void bc_d3d_output_widget::mouseReleaseEvent(QMouseEvent* p_event)
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

			platform::bc_app_event_key l_key_event(l_key, platform::bc_key_state::releasing);

			core::bc_get_service< core::bc_event_manager >()->queue_event(core::bc_make_event(l_key_event), 0);
		}

		void bc_d3d_output_widget::mouseMoveEvent(QMouseEvent* p_event)
		{
			platform::bc_pointing_device_state l_pointing_state;
			l_pointing_state.m_x = p_event->globalX();
			l_pointing_state.m_y = p_event->globalY();
			l_pointing_state.m_dx = p_event->globalX() - m_last_mouse_x;
			l_pointing_state.m_dy = p_event->globalY() - m_last_mouse_y;

			platform::bc_app_event_pointing l_pointing_event(l_pointing_state);

			core::bc_get_service< core::bc_event_manager >()->queue_event(core::bc_make_event(l_pointing_event), 0);

			m_last_mouse_x = p_event->globalX();
			m_last_mouse_y = p_event->globalY();
		}

		void bc_d3d_output_widget::keyPressEvent(QKeyEvent* p_event)
		{
			platform::bc_app_event_key l_key_event
			(
				static_cast<platform::bc_key>(p_event->nativeVirtualKey()),
				platform::bc_key_state::pressing
			);

			core::bc_get_service< core::bc_event_manager >()->queue_event(core::bc_make_event(l_key_event), 0);
		}

		void bc_d3d_output_widget::keyReleaseEvent(QKeyEvent* p_event)
		{
			if(p_event->isAutoRepeat())
			{
				return;
			}

			platform::bc_app_event_key l_key_event
			(
				static_cast<platform::bc_key>(p_event->nativeVirtualKey()),
				platform::bc_key_state::releasing
			);

			core::bc_get_service< core::bc_event_manager >()->queue_event(core::bc_make_event(l_key_event), 0);
		}

		void bc_d3d_output_widget::resizeEvent(QResizeEvent* p_resize)
		{
			platform::bc_app_event_window_resize l_resize_event
			(
				reinterpret_cast< platform::bc_window_id >(m_win_id),
				platform::bc_app_event_window_resize::state::resize,
				p_resize->size().width(),
				p_resize->size().height()
			);

			core::bc_get_service< core::bc_event_manager >()->queue_event(core::bc_make_event(l_resize_event), 0);
		}

		void bc_d3d_output_widget::focus_event(QFocusEvent* p_event)
		{
			platform::bc_app_event_window_focus l_focus_event
			(
				reinterpret_cast<platform::bc_window_id>(m_win_id),
				p_event->gotFocus()
			);

			core::bc_get_service< core::bc_event_manager >()->queue_event(core::bc_make_event(l_focus_event), 0);
		}

		void bc_d3d_output_widget::window_state_change_event(QWindowStateChangeEvent* p_event, Qt::WindowStates p_window_state)
		{
			QWindowStateChangeEvent* l_window_state_event = static_cast<QWindowStateChangeEvent*>(p_event);
			platform::bc_app_event_window_resize::state l_state = platform::bc_app_event_window_resize::state::resize;

			if (!(l_window_state_event->oldState() & Qt::WindowMaximized) && (p_window_state & Qt::WindowMaximized))
			{
				l_state = platform::bc_app_event_window_resize::state::maximized;
			}
			else if (!(l_window_state_event->oldState() & Qt::WindowMinimized) && (p_window_state & Qt::WindowMinimized))
			{
				l_state = platform::bc_app_event_window_resize::state::minimized;
			}

			platform::bc_app_event_window_resize l_resize_event
			(
				reinterpret_cast< platform::bc_window_id >(m_win_id),
				l_state,
				width(),
				height()
			);

			core::bc_get_service< core::bc_event_manager >()->queue_event(core::bc_make_event(l_resize_event), 0);
		}

		void bc_d3d_output_widget::close_event(QCloseEvent* event)
		{
			platform::bc_app_event_window_close l_close_event(reinterpret_cast<platform::bc_window_id>(m_win_id));

			core::bc_get_service< core::bc_event_manager >()->queue_event(core::bc_make_event(l_close_event), 0);
		}
	}
}