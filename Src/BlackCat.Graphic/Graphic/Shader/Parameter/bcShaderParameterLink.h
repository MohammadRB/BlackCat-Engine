// [06/30/2019 MRB]

#pragma once

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

		class bc_shader_parameter_link
		{
		public:
			bc_shader_parameter_link();

			bc_shader_parameter_link(const bc_shader_parameter_link&) = delete;

			bc_shader_parameter_link(bc_shader_parameter_link&&) = delete;

			~bc_shader_parameter_link() = default;

			bc_shader_parameter_link& operator=(const bc_shader_parameter_link&) = delete;

			bc_shader_parameter_link& operator=(bc_shader_parameter_link&&) = delete;

			void set_as_sampler(bc_sampler_state p_sampler) noexcept;

			bc_sampler_state get_as_sampler() const noexcept;

			void set_as_cbuffer(bc_buffer p_buffer) noexcept;

			bc_buffer get_as_cbuffer() const noexcept;

			void set_as_resource_view(bc_resource_view p_view) noexcept;

			bc_resource_view get_as_resource_view() const noexcept;

		private:
			bool m_has_value;
			bc_shader_parameter_type m_parameter_type;
			union
			{
				bc_sampler_state m_sampler;
				bc_buffer m_cbuffer;
				bc_resource_view m_resource_view;
			};
		};

		inline bc_shader_parameter_link::bc_shader_parameter_link()
			: m_has_value(false),
			m_parameter_type()
		{
		}

		inline void bc_shader_parameter_link::set_as_sampler(bc_sampler_state p_sampler) noexcept
		{
			if(!p_sampler.is_valid())
			{
				return;
			}

			m_has_value = true;
			m_parameter_type = bc_shader_parameter_type::sampler;
			m_sampler = p_sampler;
		}

		inline bc_sampler_state bc_shader_parameter_link::get_as_sampler() const noexcept
		{
			if(!m_has_value)
			{
				return bc_sampler_state();
			}

			bcAssert(m_parameter_type == bc_shader_parameter_type::sampler);

			return m_sampler;
		}

		inline void bc_shader_parameter_link::set_as_cbuffer(bc_buffer p_buffer) noexcept
		{
			if (!p_buffer.is_valid())
			{
				return;
			}

			m_has_value = true;
			m_parameter_type = bc_shader_parameter_type::cbuffer;
			m_cbuffer = p_buffer;
		}

		inline bc_buffer bc_shader_parameter_link::get_as_cbuffer() const noexcept
		{
			if (!m_has_value)
			{
				return bc_buffer();
			}

			bcAssert(m_parameter_type == bc_shader_parameter_type::cbuffer);

			return m_cbuffer;
		}

		inline void bc_shader_parameter_link::set_as_resource_view(bc_resource_view p_view) noexcept
		{
			if (!p_view.is_valid())
			{
				return;
			}

			m_has_value = true;
			m_parameter_type = p_view.get_view_type() == bc_resource_view_type::unordered
				                   ? bc_shader_parameter_type::unordered_view
				                   : bc_shader_parameter_type::shader_view;
			m_resource_view = p_view;
		}

		inline bc_resource_view bc_shader_parameter_link::get_as_resource_view() const noexcept
		{
			if (!m_has_value)
			{
				return bc_resource_view();
			}

			bcAssert(m_parameter_type == bc_shader_parameter_type::shader_view || m_parameter_type == bc_shader_parameter_type::unordered_view);

			return m_resource_view;
		}
	}
}