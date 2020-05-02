// [06/30/2019 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Graphic/Resource/State/bcSamplerState.h"
#include "Graphic/Resource/Buffer/bcBuffer.h"
#include "Graphic/Resource/View/bcResourceView.h"

namespace black_cat
{
	namespace graphic
	{
		enum class bc_shader_parameter_type : bcUBYTE
		{
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

			~_bc_shader_parameter_link_data();

			bc_shader_parameter_type m_parameter_type;
			union
			{
				bc_sampler_state m_sampler;
				bc_buffer m_cbuffer;
				bc_resource_view m_resource_view;
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

		inline _bc_shader_parameter_link_data::~_bc_shader_parameter_link_data()
		{
		}

		inline bc_shader_parameter_link::bc_shader_parameter_link()
			: m_data(nullptr)
		{
		}

		inline bc_shader_parameter_link::bc_shader_parameter_link(bc_shader_parameter_link&& p_other) noexcept
			: m_data(std::move(p_other.m_data))
		{
		}

		inline bc_shader_parameter_link::~bc_shader_parameter_link()
		{
		}

		inline bc_shader_parameter_link& bc_shader_parameter_link::operator=(bc_shader_parameter_link&& p_other) noexcept
		{
			m_data = std::move(p_other.m_data);

			return *this;
		}

		inline void bc_shader_parameter_link::set_as_sampler(bc_sampler_state p_sampler) noexcept
		{
			if(!p_sampler.is_valid())
			{
				return;
			}

			if (!m_data)
			{
				m_data = core::bc_make_unique<_bc_shader_parameter_link_data>(p_sampler);
			}
		}

		inline bc_sampler_state bc_shader_parameter_link::get_as_sampler() const noexcept
		{
			if(!m_data)
			{
				return bc_sampler_state();
			}

			bcAssert(m_data->m_parameter_type == bc_shader_parameter_type::sampler);

			return m_data->m_sampler;
		}

		inline void bc_shader_parameter_link::set_as_cbuffer(bc_buffer p_buffer) noexcept
		{
			if (!p_buffer.is_valid())
			{
				return;
			}

			if(!m_data)
			{
				m_data = core::bc_make_unique<_bc_shader_parameter_link_data>(p_buffer);
			}
		}

		inline bc_buffer bc_shader_parameter_link::get_as_cbuffer() const noexcept
		{
			if (!m_data)
			{
				return bc_buffer();
			}

			bcAssert(m_data->m_parameter_type == bc_shader_parameter_type::cbuffer);

			return m_data->m_cbuffer;
		}

		inline void bc_shader_parameter_link::set_as_resource_view(bc_resource_view p_view) noexcept
		{
			if (!p_view.is_valid())
			{
				return;
			}

			if (!m_data)
			{
				m_data = core::bc_make_unique<_bc_shader_parameter_link_data>(p_view);
			}
		}

		inline bc_resource_view bc_shader_parameter_link::get_as_resource_view() const noexcept
		{
			if (!m_data)
			{
				return bc_resource_view();
			}

			bcAssert(m_data->m_parameter_type == bc_shader_parameter_type::shader_view || m_data->m_parameter_type == bc_shader_parameter_type::unordered_view);

			return m_data->m_resource_view;
		}
	}
}