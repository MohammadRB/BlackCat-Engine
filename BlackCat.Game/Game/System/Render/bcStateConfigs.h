// [06/08/2016 MRB]

#pragma once

#include "Core/Container/bcArray.h"
#include "Graphic/Resource/State/bcBlendState.h"
#include "Graphic/Resource/State/bcDepthStencilState.h"
#include "Graphic/Resource/State/bcInputLayout.h"
#include "Graphic/Resource/State/bcRasterizerState.h"
#include "Graphic/Resource/State/bcViewPort.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "Game/System/Render/bcVertexLayout.h"

namespace black_cat
{
	namespace game
	{
		using bc_surface_format_type = graphic::bc_format;

		// Filtering modes order: min, mag, mip
		// Addressing modes order: u, v, w
		enum class bc_sampler_type
		{
			filter_linear_linear_linear_address_wrap_wrap_wrap
		};

		enum class bc_blend_type
		{
			opaque = 0
		};

		enum class bc_depth_stencil_type
		{
			depth_less_stencil_off = 0
		};

		enum class bc_rasterizer_type
		{
			fill_solid_cull_back = 0
		};

		enum class bc_multi_sample_type
		{
			c1_q1,
		};

		class bc_graphic_state_configs
		{
		public:
			static graphic::bc_sampler_state_config bc_sampler_config(bc_sampler_type p_value)
			{
				graphic::bc_sampler_state_config l_config;

				switch (p_value)
				{
				case bc_sampler_type::filter_linear_linear_linear_address_wrap_wrap_wrap:

					l_config.m_filter = graphic::bc_filter::min_mag_mip_linear;
					l_config.m_address_u = graphic::bc_texture_address_mode::wrap;
					l_config.m_address_v = graphic::bc_texture_address_mode::wrap;
					l_config.m_address_w = graphic::bc_texture_address_mode::wrap;
					l_config.m_mip_lod_bias = 0.0f;
					l_config.m_max_anisotropy = 1;
					l_config.m_comparison_func = graphic::bc_comparison_func::never;
					l_config.m_border_color[0] = 1.0f;
					l_config.m_border_color[1] = 1.0f;
					l_config.m_border_color[2] = 1.0f;
					l_config.m_border_color[3] = 1.0f;
					l_config.m_min_lod = 0;
					l_config.m_max_lod = std::numeric_limits<bcFLOAT>::max();
					break;
				}

				return l_config;
			}

			static graphic::bc_blend_state_config bc_blend_config(bc_blend_type p_value)
			{
				graphic::bc_blend_state_config l_config;

				switch (p_value)
				{
				case bc_blend_type::opaque:

					l_config.m_alpha_to_coverage_enable = false;
					l_config.m_independent_blend_enable = false;
					for (bcINT i = 0; i < graphic::bc_render_api_info::number_of_om_render_target_slots(); i++)
					{
						l_config.m_render_target[i].m_blend_enable = false;
						l_config.m_render_target[i].m_src_blend = graphic::bc_blend::one;
						l_config.m_render_target[i].m_dest_blend = graphic::bc_blend::zero;
						l_config.m_render_target[i].m_blend_op = graphic::bc_blend_op::add;
						l_config.m_render_target[i].m_src_blend_alpha = graphic::bc_blend::one;
						l_config.m_render_target[i].m_dest_blend_alpha = graphic::bc_blend::zero;
						l_config.m_render_target[i].m_blend_op_alpha = graphic::bc_blend_op::add;
						l_config.m_render_target[i].m_render_target_write_mask = static_cast<bcUINT8>(graphic::bc_color_write::all);
					}
					break;
				}

				return l_config;
			}

			static graphic::bc_depth_stencil_state_config bc_depth_stencil_config(bc_depth_stencil_type p_value)
			{
				graphic::bc_depth_stencil_state_config l_config;

				switch (p_value)
				{
				case bc_depth_stencil_type::depth_less_stencil_off:

					l_config.m_depth_enable = true;
					l_config.m_depth_write_mask = graphic::bc_depth_write_mask::all;
					l_config.m_depth_func = graphic::bc_comparison_func::less;
					l_config.m_stencil_enable = false;
					l_config.m_stencil_read_mask = 0xff;
					l_config.m_stencil_write_mask = 0xff;

					l_config.m_front_face.m_stencil_func = graphic::bc_comparison_func::always;
					l_config.m_front_face.m_stencil_pass_op = graphic::bc_stencil_op::keep;
					l_config.m_front_face.m_stencil_fail_op = graphic::bc_stencil_op::keep;
					l_config.m_front_face.m_stencil_depth_fail_op = graphic::bc_stencil_op::keep;

					l_config.m_back_face.m_stencil_func = graphic::bc_comparison_func::always;
					l_config.m_back_face.m_stencil_pass_op = graphic::bc_stencil_op::keep;
					l_config.m_back_face.m_stencil_fail_op = graphic::bc_stencil_op::keep;
					l_config.m_back_face.m_stencil_depth_fail_op = graphic::bc_stencil_op::keep;
					break;
				}

				return l_config;
			}

			static graphic::bc_rasterizer_state_config bc_rasterizer_config(bc_rasterizer_type p_value)
			{
				graphic::bc_rasterizer_state_config l_config;

				switch (p_value)
				{
				case bc_rasterizer_type::fill_solid_cull_back:

					l_config.m_fill_mode = graphic::bc_fill_mode::solid;
					l_config.m_cull_mode = graphic::bc_cull_mode::back;
					l_config.m_front_counter_clockwise = false;
					l_config.m_depth_bias = 0;
					l_config.m_slope_scaled_depth_bias = 0.0f;
					l_config.m_depth_bias_clamp = 0.0f;
					l_config.m_depth_clip_enable = true;
					l_config.m_scissor_enable = false;
					l_config.m_multisample_enable = false;
					l_config.m_antialiased_line_enable = false;
					break;
				}

				return l_config;
			}

			static graphic::bc_input_layout_config bc_input_layout_config(bc_vertex_type p_value)
			{
				graphic::bc_input_layout_config l_config;

				switch (p_value)
				{
				case bc_vertex_type::pos_tex:

					l_config.m_input_elements.assign
						(
					{
						graphic::bc_input_element_desc{ "POSITION", 0, graphic::bc_format::R32G32B32_FLOAT, 0, 0, graphic::bc_input_classification::per_vertex_data, 0 },
						graphic::bc_input_element_desc{ "TEXCOORD", 0, graphic::bc_format::R32G32_FLOAT, 0, 12, graphic::bc_input_classification::per_vertex_data, 0 }
					}
					);
					break;
				case bc_vertex_type::pos_tex_nor:

					l_config.m_input_elements.assign
						(
					{
						graphic::bc_input_element_desc{ "POSITION", 0, graphic::bc_format::R32G32B32_FLOAT, 0, 0, graphic::bc_input_classification::per_vertex_data, 0 },
						graphic::bc_input_element_desc{ "TEXCOORD", 0, graphic::bc_format::R32G32_FLOAT, 0, 12, graphic::bc_input_classification::per_vertex_data, 0 },
							graphic::bc_input_element_desc{ "NORMAL", 0, graphic::bc_format::R32G32B32_FLOAT, 0, 20, graphic::bc_input_classification::per_vertex_data, 0 },
					}
					);
					break;
				case bc_vertex_type::pos_tex_nor_tan:

					l_config.m_input_elements.assign
						(
					{
						graphic::bc_input_element_desc{ "POSITION", 0, graphic::bc_format::R32G32B32_FLOAT, 0, 0, graphic::bc_input_classification::per_vertex_data, 0 },
						graphic::bc_input_element_desc{ "TEXCOORD", 0, graphic::bc_format::R32G32_FLOAT, 0, 12, graphic::bc_input_classification::per_vertex_data, 0 },
							graphic::bc_input_element_desc{ "NORMAL", 0, graphic::bc_format::R32G32B32_FLOAT, 0, 20, graphic::bc_input_classification::per_vertex_data, 0 },
							graphic::bc_input_element_desc{ "TANGENT", 0, graphic::bc_format::R32G32B32_FLOAT, 0, 32, graphic::bc_input_classification::per_vertex_data, 0 },
					}
					);
					break;
				}

				return l_config;
			}

			static graphic::bc_texture_ms_config bc_texture_multi_sample(bc_multi_sample_type p_ms)
			{
				graphic::bc_texture_ms_config l_config(1, 1);

				switch (p_ms)
				{
				case bc_multi_sample_type::c1_q1:
					l_config = graphic::bc_texture_ms_config(1, 1);
					break;
				}

				return l_config;
			}

		protected:

		private:
		};
	}
}