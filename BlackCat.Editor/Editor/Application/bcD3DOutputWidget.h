// [11/13/2016 MRB]

#pragma once

#include "GraphicImp/Device/bcDeviceOutput.h"
#include <QWidget>

namespace black_cat
{
	namespace editor
	{
		class bc_editor_app;

		class bc_d3d_output_widget : public QWidget
		{
		public:
			Q_OBJECT
			Q_DISABLE_COPY(bc_d3d_output_widget)
			friend class bc_editor_app;

		public:
			explicit bc_d3d_output_widget(QWidget* p_parent = nullptr);

			~bc_d3d_output_widget();

			QPaintEngine* paintEngine() const override;

			graphic::bc_device_output get_device_output() const;

		protected:
			void mousePressEvent(QMouseEvent* p_event) override;

			void mouseReleaseEvent(QMouseEvent* p_event) override;

			void mouseMoveEvent(QMouseEvent* p_event) override;

			void keyPressEvent(QKeyEvent* p_event) override;

			void keyReleaseEvent(QKeyEvent* p_event) override;

			void resizeEvent(QResizeEvent* p_resize) override;

			void focus_event(QFocusEvent* p_event);

			void window_state_change_event(QWindowStateChangeEvent* p_event, Qt::WindowStates p_window_state);

			void close_event(QCloseEvent* event);

		private:
			HWND m_win_id;

			int m_last_mouse_x;
			int m_last_mouse_y;
		};
	}
}
