// [13/11/2016 MRB]

#pragma once

#include "GraphicImp/Device/bcDeviceOutput.h"
#include <QtWidgets/QWidget>
#include <QtGui/QtEvents>

class QMainWindow;

namespace black_cat::editor
{
	class bc_editor_app;

	class bc_widget_d3d_output : public QWidget
	{
	public:
		Q_OBJECT
		Q_DISABLE_COPY(bc_widget_d3d_output)
		friend class bc_editor_app;

	public:
		bc_widget_d3d_output(QMainWindow* p_main_window, QWidget* p_parent = nullptr);

		~bc_widget_d3d_output() override;

		QPaintEngine* paintEngine() const override;

		graphic::bc_device_output get_device_output() const;

		void set_editor_mode(bool p_value) noexcept;

		void request_resize(bcUINT p_width, bcUINT p_height) noexcept;

		void update_ui();

	signals:
		void mousePressed(QMouseEvent* p_event);

		void mouseReleased(QMouseEvent* p_event);

		void mouseMoved(QMouseEvent* p_event);

	private:
		void mousePressEvent(QMouseEvent* p_event) override;

		void mouseReleaseEvent(QMouseEvent* p_event) override;

		void mouseMoveEvent(QMouseEvent* p_event) override;

		void keyPressEvent(QKeyEvent* p_event) override;

		void keyReleaseEvent(QKeyEvent* p_event) override;

		void resizeEvent(QResizeEvent* p_resize) override;

		void window_initialized();

		void focus_event(QFocusEvent* p_event);

		void window_state_change_event(QWindowStateChangeEvent* p_event, Qt::WindowStates p_window_state);

		void close_event(QCloseEvent* p_event);

		QMainWindow* m_main_window;
		HWND m_win_id;
		bool m_editor_mode;
		bcINT m_last_mouse_x;
		bcINT m_last_mouse_y;

		bool m_request_resize;
		bcUINT m_resize_x;
		bcUINT m_resize_y;
	};
}
