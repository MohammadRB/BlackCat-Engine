// [30/06/2019 MRB]

#pragma once

#include <variant>
#include "Core/Memory/bcPtr.h"
#include "Graphic/Resource/State/bcSamplerState.h"
#include "Graphic/Resource/Buffer/bcBuffer.h"
#include "Graphic/Resource/View/bcResourceView.h"
#include "Graphic/Resource/View/bcRenderTargetView.h"

namespace black_cat::graphic
{
	enum class bc_shader_parameter_type : bcUBYTE
	{
		render_target_view,
		shader_view,
		unordered_view,
		cbuffer,
		sampler
	};

	struct _bc_shader_parameter_link_data
	{
		constexpr _bc_shader_parameter_link_data() noexcept;

		_bc_shader_parameter_link_data(bc_sampler_state p_sampler) noexcept;

		_bc_shader_parameter_link_data(bc_buffer p_buffer) noexcept;

		_bc_shader_parameter_link_data(bc_resource_view p_view) noexcept;

		_bc_shader_parameter_link_data(bc_render_target_view p_view) noexcept;

		_bc_shader_parameter_link_data(const _bc_shader_parameter_link_data& p_other) noexcept;
			
		~_bc_shader_parameter_link_data() noexcept;

		_bc_shader_parameter_link_data& operator=(const _bc_shader_parameter_link_data& p_other) noexcept;

		bc_shader_parameter_type m_parameter_type;
		std::variant
		<
			std::monostate,
			bc_sampler_state,
			bc_buffer,
			bc_resource_view,
			bc_render_target_view
		> m_parameter_holder;
	};

	class bc_shader_parameter_link
	{
	public:
		bc_shader_parameter_link() noexcept;

		bc_shader_parameter_link(const bc_shader_parameter_link&) noexcept;

		bc_shader_parameter_link(bc_shader_parameter_link&& p_other) noexcept;

		~bc_shader_parameter_link() noexcept;

		bc_shader_parameter_link& operator=(const bc_shader_parameter_link&) noexcept;

		bc_shader_parameter_link& operator=(bc_shader_parameter_link&& p_other) noexcept;

		void set_as_sampler(bc_sampler_state p_sampler) noexcept;

		bc_sampler_state get_as_sampler() const noexcept;

		void set_as_cbuffer(bc_buffer p_buffer) noexcept;

		bc_buffer get_as_cbuffer() const noexcept;

		void set_as_resource_view(bc_resource_view p_view) noexcept;

		bc_resource_view get_as_resource_view() const noexcept;

		void set_as_render_target_view(bc_render_target_view p_view) noexcept;

		bc_render_target_view get_as_render_target_view() const noexcept;

		bool is_valid() const noexcept;

	private:
		core::bc_shared_ptr<_bc_shader_parameter_link_data> m_data;
	};

	constexpr _bc_shader_parameter_link_data::_bc_shader_parameter_link_data() noexcept
		: m_parameter_type(bc_shader_parameter_type::sampler),
		  m_parameter_holder(std::monostate())
	{
	}

	inline _bc_shader_parameter_link_data::_bc_shader_parameter_link_data(bc_sampler_state p_sampler) noexcept
		: m_parameter_type(bc_shader_parameter_type::sampler),
		  m_parameter_holder(p_sampler)
	{
	}

	inline _bc_shader_parameter_link_data::_bc_shader_parameter_link_data(bc_buffer p_buffer) noexcept
		: m_parameter_type(bc_shader_parameter_type::cbuffer),
		  m_parameter_holder(p_buffer)
	{
	}

	inline _bc_shader_parameter_link_data::_bc_shader_parameter_link_data(bc_resource_view p_view) noexcept
		: m_parameter_type(bc_shader_parameter_type::shader_view),
		  m_parameter_holder(p_view)
	{
		if (p_view.get_view_type() == bc_resource_view_type::unordered)
		{
			m_parameter_type = bc_shader_parameter_type::unordered_view;
		}
	}

	inline _bc_shader_parameter_link_data::_bc_shader_parameter_link_data(bc_render_target_view p_view) noexcept
		: m_parameter_type(bc_shader_parameter_type::render_target_view),
		  m_parameter_holder(p_view)
	{
	}

	inline _bc_shader_parameter_link_data::_bc_shader_parameter_link_data(const _bc_shader_parameter_link_data& p_other) noexcept = default;

	inline _bc_shader_parameter_link_data::~_bc_shader_parameter_link_data() noexcept = default;

	inline _bc_shader_parameter_link_data& _bc_shader_parameter_link_data::operator=(const _bc_shader_parameter_link_data& p_other) noexcept = default;

	inline bc_shader_parameter_link::bc_shader_parameter_link() noexcept
		: m_data(core::bc_make_shared<_bc_shader_parameter_link_data>())
	{
	}

	inline bc_shader_parameter_link::bc_shader_parameter_link(const bc_shader_parameter_link&) noexcept = default;

	inline bc_shader_parameter_link::bc_shader_parameter_link(bc_shader_parameter_link&& p_other) noexcept = default;

	inline bc_shader_parameter_link::~bc_shader_parameter_link() noexcept = default;

	inline bc_shader_parameter_link& bc_shader_parameter_link::operator=(const bc_shader_parameter_link&) noexcept = default;

	inline bc_shader_parameter_link& bc_shader_parameter_link::operator=(bc_shader_parameter_link&& p_other) noexcept = default;

	inline void bc_shader_parameter_link::set_as_sampler(bc_sampler_state p_sampler) noexcept
	{
		*m_data = _bc_shader_parameter_link_data(p_sampler);
	}

	inline bc_sampler_state bc_shader_parameter_link::get_as_sampler() const noexcept
	{
		if (!is_valid())
		{
			return {};
		}

		BC_ASSERT(m_data->m_parameter_type == bc_shader_parameter_type::sampler);

		auto* l_sampler = std::get_if<bc_sampler_state>(&m_data->m_parameter_holder);
		return l_sampler ? *l_sampler : bc_sampler_state();
	}

	inline void bc_shader_parameter_link::set_as_cbuffer(bc_buffer p_buffer) noexcept
	{
		*m_data = _bc_shader_parameter_link_data(p_buffer);
	}

	inline bc_buffer bc_shader_parameter_link::get_as_cbuffer() const noexcept
	{
		if (!is_valid())
		{
			return {};
		}

		BC_ASSERT(m_data->m_parameter_type == bc_shader_parameter_type::cbuffer);

		auto* l_cbuffer = std::get_if<bc_buffer>(&m_data->m_parameter_holder);
		return l_cbuffer ? *l_cbuffer : bc_buffer();
	}

	inline void bc_shader_parameter_link::set_as_resource_view(bc_resource_view p_view) noexcept
	{
		*m_data = _bc_shader_parameter_link_data(p_view);
	}

	inline bc_resource_view bc_shader_parameter_link::get_as_resource_view() const noexcept
	{
		if (!is_valid())
		{
			return {};
		}

		BC_ASSERT(m_data->m_parameter_type == bc_shader_parameter_type::shader_view || m_data->m_parameter_type == bc_shader_parameter_type::unordered_view);

		auto* l_view = std::get_if<bc_resource_view>(&m_data->m_parameter_holder);
		return l_view ? *l_view : bc_resource_view();
	}

	inline void bc_shader_parameter_link::set_as_render_target_view(bc_render_target_view p_view) noexcept
	{
		*m_data = _bc_shader_parameter_link_data(p_view);
	}

	inline bc_render_target_view bc_shader_parameter_link::get_as_render_target_view() const noexcept
	{
		if (!is_valid())
		{
			return {};
		}

		BC_ASSERT(m_data->m_parameter_type == bc_shader_parameter_type::render_target_view);

		auto* l_view = std::get_if<bc_render_target_view>(&m_data->m_parameter_holder);
		return l_view ? *l_view : bc_render_target_view();
	}

	inline bool bc_shader_parameter_link::is_valid() const noexcept
	{
		return m_data->m_parameter_holder.index() != 0;
	}
}
