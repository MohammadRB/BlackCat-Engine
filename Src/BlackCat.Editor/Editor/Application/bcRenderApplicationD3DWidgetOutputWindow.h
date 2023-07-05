// [13/11/2016 MRB]

#pragma once

#include "Game/Application/bcIRenderApplicationOutputWindow.h"
#include "Editor/Widget/bcWidgetD3DOutput.h"

namespace black_cat::editor
{
	class bc_render_application_d3dwidget_output_window : public game::bci_render_application_output_window
	{
	public:
		bc_render_application_d3dwidget_output_window(bc_widget_d3d_output* p_d3d_widget);

		bc_render_application_d3dwidget_output_window(bc_render_application_d3dwidget_output_window&&) noexcept;

		~bc_render_application_d3dwidget_output_window() override;

		bc_render_application_d3dwidget_output_window& operator=(bc_render_application_d3dwidget_output_window&&) noexcept;

		bc_widget_d3d_output* get_widget() noexcept;

		platform::bc_window::id get_id() const noexcept override;

		bcUINT get_width() const noexcept override;
			
		bcUINT get_height() const noexcept override;

		bcUINT get_left() const noexcept override;
			
		bcUINT get_top() const noexcept override;

		platform::bc_window_state get_state() const noexcept override;

		graphic::bc_device_output get_device_output() const override;

		void set_size(bcUINT p_width, bcUINT p_height) override;

		void update() override;

		void close() noexcept override;

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

	inline platform::bc_window::id bc_render_application_d3dwidget_output_window::get_id() const noexcept
	{
		return static_cast<platform::bc_window::id>(m_d3d_widget->winId());
	}
		
	inline bcUINT bc_render_application_d3dwidget_output_window::get_width() const noexcept
	{
		return m_d3d_widget->width();
	}
		
	inline bcUINT bc_render_application_d3dwidget_output_window::get_height() const noexcept
	{
		return  m_d3d_widget->height();
	}

	inline bcUINT bc_render_application_d3dwidget_output_window::get_left() const noexcept
	{
		return m_d3d_widget->pos().x();
	}

	inline bcUINT bc_render_application_d3dwidget_output_window::get_top() const noexcept
	{
		return m_d3d_widget->pos().y();
	}

	inline platform::bc_window_state bc_render_application_d3dwidget_output_window::get_state() const noexcept
	{
		return platform::bc_window_state::normal;
	}

	inline graphic::bc_device_output bc_render_application_d3dwidget_output_window::get_device_output() const
	{
		return m_d3d_widget->get_device_output();
	}

	inline void bc_render_application_d3dwidget_output_window::set_size(bcUINT p_width, bcUINT p_height)
	{
		m_d3d_widget->request_resize(p_width, p_height);
	}

	inline void bc_render_application_d3dwidget_output_window::update()
	{
		// Because this function will be called by engine thread we cannot update anything related to UI
	}

	inline void bc_render_application_d3dwidget_output_window::close() noexcept
	{
		// Because this function will be called by engine thread we cannot update anything related to UI
	}
}
