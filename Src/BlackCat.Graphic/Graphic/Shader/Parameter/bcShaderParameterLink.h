// [06/30/2019 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Graphic/Resource/State/bcSamplerState.h"
#include "Graphic/Resource/Buffer/bcBuffer.h"
#include "Graphic/Resource/View/bcResourceView.h"
#include "Graphic/Resource/View/bcRenderTargetView.h"

namespace black_cat
{
	namespace graphic
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
			_bc_shader_parameter_link_data(bc_sampler_state p_sampler);

			_bc_shader_parameter_link_data(bc_buffer p_buffer);

			_bc_shader_parameter_link_data(bc_resource_view p_view);

			_bc_shader_parameter_link_data(bc_render_target_view p_view);

			_bc_shader_parameter_link_data(const _bc_shader_parameter_link_data& p_other);
			
			~_bc_shader_parameter_link_data();

			_bc_shader_parameter_link_data& operator=(const _bc_shader_parameter_link_data& p_other);

			bc_shader_parameter_type m_parameter_type;
			union
			{
				bc_sampler_state m_sampler;
				bc_buffer m_cbuffer;
				bc_resource_view m_resource_view;
				bc_render_target_view m_render_target_view;
			};
		};

		class bc_shader_parameter_link
		{
		public:
			bc_shader_parameter_link();

			bc_shader_parameter_link(const bc_shader_parameter_link&) = delete;

			bc_shader_parameter_link(bc_shader_parameter_link&& p_other) noexcept;

			~bc_shader_parameter_link();

			bc_shader_parameter_link& operator=(const bc_shader_parameter_link&) = delete;

			bc_shader_parameter_link& operator=(bc_shader_parameter_link&& p_other) noexcept;

			void set_as_sampler(bc_sampler_state p_sampler) noexcept;

			bc_sampler_state get_as_sampler() const noexcept;

			void set_as_cbuffer(bc_buffer p_buffer) noexcept;

			bc_buffer get_as_cbuffer() const noexcept;

			void set_as_resource_view(bc_resource_view p_view) noexcept;

			bc_resource_view get_as_resource_view() const noexcept;

			void set_as_render_target_view(bc_render_target_view p_view) noexcept;

			bc_render_target_view get_as_render_target_view() const noexcept;

		private:
			core::bc_unique_ptr<_bc_shader_parameter_link_data> m_data;
		};

		inline _bc_shader_parameter_link_data::_bc_shader_parameter_link_data(bc_sampler_state p_sampler):
			m_parameter_type(bc_shader_parameter_type::sampler),
			m_sampler(p_sampler)
		{
		}

		inline _bc_shader_parameter_link_data::_bc_shader_parameter_link_data(bc_buffer p_buffer)
			: m_parameter_type(bc_shader_parameter_type::cbuffer),
			m_cbuffer(p_buffer)
		{
		}

		inline _bc_shader_parameter_link_data::_bc_shader_parameter_link_data(bc_resource_view p_view)
			: m_parameter_type(bc_shader_parameter_type::shader_view),
			m_resource_view(p_view)
		{
			if (p_view.get_view_type() == bc_resource_view_type::unordered)
			{
				m_parameter_type = bc_shader_parameter_type::unordered_view;
			}
		}

		inline _bc_shader_parameter_link_data::_bc_shader_parameter_link_data(bc_render_target_view p_view)
			: m_parameter_type(bc_shader_parameter_type::render_target_view),
			m_render_target_view(p_view)
		{
		}

		inline _bc_shader_parameter_link_data::_bc_shader_parameter_link_data(const _bc_shader_parameter_link_data& p_other)
		{
			operator=(p_other);
		}

		inline _bc_shader_parameter_link_data::~_bc_shader_parameter_link_data()
		{
			switch (m_parameter_type)
			{
			case bc_shader_parameter_type::shader_view:
			case bc_shader_parameter_type::unordered_view:
				m_resource_view.~bc_resource_view();
				break;
			case bc_shader_parameter_type::cbuffer:
				m_cbuffer.~bc_buffer();
				break;
			case bc_shader_parameter_type::sampler:
				m_sampler.~bc_sampler_state();
				break;
			case bc_shader_parameter_type::render_target_view:
				m_render_target_view.~bc_platform_render_target_view();
				break;
			default: 
				BC_ASSERT(false);
			}
		}

		inline _bc_shader_parameter_link_data& _bc_shader_parameter_link_data::operator=(const _bc_shader_parameter_link_data& p_other)
		{
			m_parameter_type = p_other.m_parameter_type;
			
			switch (m_parameter_type)
			{
			case bc_shader_parameter_type::shader_view:
			case bc_shader_parameter_type::unordered_view:
				m_resource_view = p_other.m_resource_view;
				break;
			case bc_shader_parameter_type::cbuffer:
				m_cbuffer = p_other.m_cbuffer;
				break;
			case bc_shader_parameter_type::sampler:
				m_sampler = p_other.m_sampler;
				break;
			case bc_shader_parameter_type::render_target_view:
				m_render_target_view = p_other.m_render_target_view;
				break;
			default: 
				BC_ASSERT(false);
			}

			return *this;
		}

		inline bc_shader_parameter_link::bc_shader_parameter_link()
			: m_data(nullptr)
		{
		}

		inline bc_shader_parameter_link::bc_shader_parameter_link(bc_shader_parameter_link&& p_other) noexcept
			: m_data(std::move(p_other.m_data))
		{
		}

		inline bc_shader_parameter_link::~bc_shader_parameter_link() = default;

		inline bc_shader_parameter_link& bc_shader_parameter_link::operator=(bc_shader_parameter_link&& p_other) noexcept
		{
			m_data = std::move(p_other.m_data);

			return *this;
		}

		inline void bc_shader_parameter_link::set_as_sampler(bc_sampler_state p_sampler) noexcept
		{
			if (!m_data)
			{
				m_data = core::bc_make_unique<_bc_shader_parameter_link_data>(p_sampler);
			}
			else
			{
				*m_data = _bc_shader_parameter_link_data(p_sampler);
			}
		}

		inline bc_sampler_state bc_shader_parameter_link::get_as_sampler() const noexcept
		{
			if(!m_data)
			{
				return bc_sampler_state();
			}

			BC_ASSERT(m_data->m_parameter_type == bc_shader_parameter_type::sampler);

			return m_data->m_sampler;
		}

		inline void bc_shader_parameter_link::set_as_cbuffer(bc_buffer p_buffer) noexcept
		{
			if(!m_data)
			{
				m_data = core::bc_make_unique<_bc_shader_parameter_link_data>(p_buffer);
			}
			else
			{
				*m_data = _bc_shader_parameter_link_data(p_buffer);
			}
		}

		inline bc_buffer bc_shader_parameter_link::get_as_cbuffer() const noexcept
		{
			if (!m_data)
			{
				return bc_buffer();
			}

			BC_ASSERT(m_data->m_parameter_type == bc_shader_parameter_type::cbuffer);

			return m_data->m_cbuffer;
		}

		inline void bc_shader_parameter_link::set_as_resource_view(bc_resource_view p_view) noexcept
		{
			if (!m_data)
			{
				m_data = core::bc_make_unique<_bc_shader_parameter_link_data>(p_view);
			}
			else
			{
				*m_data = _bc_shader_parameter_link_data(p_view);
			}
		}

		inline bc_resource_view bc_shader_parameter_link::get_as_resource_view() const noexcept
		{
			if (!m_data)
			{
				return bc_resource_view();
			}

			BC_ASSERT(m_data->m_parameter_type == bc_shader_parameter_type::shader_view || m_data->m_parameter_type == bc_shader_parameter_type::unordered_view);

			return m_data->m_resource_view;
		}

		inline void bc_shader_parameter_link::set_as_render_target_view(bc_render_target_view p_view) noexcept
		{
			if (!m_data)
			{
				m_data = core::bc_make_unique<_bc_shader_parameter_link_data>(p_view);
			}
			else
			{
				*m_data = _bc_shader_parameter_link_data(p_view);
			}
		}

		inline bc_render_target_view bc_shader_parameter_link::get_as_render_target_view() const noexcept
		{
			if (!m_data)
			{
				return bc_render_target_view();
			}

			BC_ASSERT(m_data->m_parameter_type == bc_shader_parameter_type::render_target_view);

			return m_data->m_render_target_view;
		}
	}
}