// [06/30/2019 MRB]

#pragma once

#include "Graphic/Resource/State/bcSamplerState.h"
#include "Graphic/Resource/Buffer/bcBuffer.h"
#include "Graphic/Resource/View/bcResourceView.h"

namespace black_cat
{
	namespace graphic
	{
		enum class bc_shader_parameter_type : bcBYTE
		{
			shader_view,
			unordered_view,
			cbuffer,
			sampler
		};

		class bc_shader_parameter_link
		{
		public:
			bc_shader_parameter_link() = default;

			bc_shader_parameter_link(const bc_shader_parameter_link&) = delete;

			bc_shader_parameter_link(bc_shader_parameter_link&&) = delete;

			~bc_shader_parameter_link() = default;

			bc_shader_parameter_link& operator=(const bc_shader_parameter_link&) = delete;

			bc_shader_parameter_link& operator=(bc_shader_parameter_link&&) = delete;

			void set_as_sampler_parameter(bc_sampler_state p_sampler) noexcept;

			bc_sampler_state get_as_sampler_parameter() const noexcept;

			void set_as_constant_buffer_parameter(bc_buffer p_buffer) noexcept;

			bc_buffer get_as_constant_buffer_parameter() const noexcept;

			void set_as_resource_view_parameter(bc_resource_view p_view) noexcept;

			bc_resource_view get_as_resource_view_parameter() const noexcept;

		private:
			bc_shader_parameter_type m_type;
			union
			{
				bc_sampler_state m_sampler;
				bc_buffer m_cbuffer;
				bc_resource_view m_resource_view;
			};
		};
	}
}