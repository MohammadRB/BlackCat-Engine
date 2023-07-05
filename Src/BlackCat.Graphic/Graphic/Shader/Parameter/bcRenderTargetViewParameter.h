// [16/01/2016 MRB]

#pragma once

#include "Graphic/Resource/View/bcRenderTargetView.h"

namespace black_cat::graphic
{
	class bc_render_target_view_parameter
	{
	public:
		bc_render_target_view_parameter();

		explicit bc_render_target_view_parameter(bc_render_target_view p_render_target);

		explicit bc_render_target_view_parameter(bc_shader_parameter_link p_parameter_link);

		bc_render_target_view_parameter(const bc_render_target_view_parameter&) = default;

		~bc_render_target_view_parameter() = default;

		bc_render_target_view_parameter& operator=(const bc_render_target_view_parameter&) = default;

		bc_render_target_view get_render_target_view() const;

		void set_render_target_view(bc_render_target_view p_view);

		bool is_valid() const;

	private:
		bc_render_target_view m_render_target_view;
		bc_shader_parameter_link m_link;
	};

	inline bc_render_target_view_parameter::bc_render_target_view_parameter()
		: m_render_target_view(),
		  m_link()
	{
	}

	inline bc_render_target_view_parameter::bc_render_target_view_parameter(bc_render_target_view p_render_target)
		: m_render_target_view(p_render_target),
		  m_link()
	{
	}

	inline bc_render_target_view_parameter::bc_render_target_view_parameter(bc_shader_parameter_link p_parameter_link)
		: m_render_target_view(),
		  m_link(std::move(p_parameter_link))
	{
	}

	inline bc_render_target_view bc_render_target_view_parameter::get_render_target_view() const
	{
		return m_render_target_view.is_valid() ? m_render_target_view : m_link.get_as_render_target_view();
	}

	inline void bc_render_target_view_parameter::set_render_target_view(bc_render_target_view p_view)
	{
		m_render_target_view = p_view;
	}

	inline bool bc_render_target_view_parameter::is_valid() const
	{
		return get_render_target_view().is_valid();
	}
}
