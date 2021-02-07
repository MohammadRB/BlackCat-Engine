// [11/13/2016 MRB]

#pragma once

#include "Game/Application/bcIRenderApplicationOutputWindow.h"
#include "Editor/Widget/bcWidgetD3DOutput.h"

namespace black_cat
{
	namespace editor
	{
		class bc_render_application_d3dwidget_output_window : public game::bci_render_application_output_window
		{
		public:
			bc_render_application_d3dwidget_output_window(bc_widget_d3d_output* p_d3d_widget);

			bc_render_application_d3dwidget_output_window(bc_render_application_d3dwidget_output_window&&) noexcept;

			~bc_render_application_d3dwidget_output_window();

			bc_render_application_d3dwidget_output_window& operator=(bc_render_application_d3dwidget_output_window&&) noexcept;

			bc_widget_d3d_output* get_widget() noexcept;

			platform::bc_window_id get_id() const noexcept override;

			bcUINT32 get_width() const noexcept override;

			bcUINT32 get_height() const noexcept override;

			graphic::bc_device_output get_device_output() const override;

			void update() override;

			void close() noexcept override;

		protected:

		private:
			bc_widget_d3d_output* m_d3d_widget;
		};

		inline bc_render_application_d3dwidget_output_window::bc_render_application_d3dwidget_output_window(bc_widget_d3d_output* p_d3d_widget)
			: m_d3d_widget(p_d3d_widget)
		{
		}

		inline bc_render_application_d3dwidget_output_window::bc_render_application_d3dwidget_output_window(bc_render_application_d3dwidget_output_window&&) noexcept = default;

		inline bc_render_application_d3dwidget_output_window::~bc_render_application_d3dwidget_output_window() = default;

		inline bc_render_application_d3dwidget_output_window& bc_render_application_d3dwidget_output_window::operator=(bc_render_application_d3dwidget_output_window&&) noexcept = default;

		inline bc_widget_d3d_output* bc_render_application_d3dwidget_output_window::get_widget() noexcept
		{
			return m_d3d_widget;
		}

		inline platform::bc_window_id bc_render_application_d3dwidget_output_window::get_id() const noexcept
		{
			return static_cast< platform::bc_window_id >(m_d3d_widget->winId());
		}

		inline bcUINT32 bc_render_application_d3dwidget_output_window::get_width() const noexcept
		{
			return m_d3d_widget->width();
		}

		inline bcUINT32 bc_render_application_d3dwidget_output_window::get_height() const noexcept
		{
			return  m_d3d_widget->height();
		}

		inline graphic::bc_device_output bc_render_application_d3dwidget_output_window::get_device_output() const
		{
			return m_d3d_widget->get_device_output();
		}

		inline void bc_render_application_d3dwidget_output_window::update()
		{
			// Because this thread will be called by engine thread we cannot update anything related to UI
		}

		inline void bc_render_application_d3dwidget_output_window::close() noexcept
		{
			// Because this thread will be called by engine thread we cannot update anything related to UI
			//m_d3d_widget->close();
		}
	}
}