// [12/06/2018 MRB]

#include "Game/GamePCH.h"
#include "Game/bcException.h"
#include "Game/System/Render/State/bcStateConfigs.h"

namespace black_cat
{
	namespace game
	{
		graphic::bc_blend_state_config bc_graphic_state_configs::bc_blend_config(bc_blend_type p_value)
		{
			graphic::bc_blend_state_config l_config;

			switch (p_value)
			{
			case bc_blend_type::opaque:
			{
				l_config.m_alpha_to_coverage_enable = false;
				l_config.m_independent_blend_enable = false;
				for (bcUINT32 i = 0; i < graphic::bc_render_api_info::number_of_om_render_target_slots(); i++)
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
			case bc_blend_type::blending_overwrite_alpha:
			{
				l_config.m_alpha_to_coverage_enable = false;
				l_config.m_independent_blend_enable = false;
				for (bcUINT32 i = 0; i < graphic::bc_render_api_info::number_of_om_render_target_slots(); i++)
				{
					l_config.m_render_target[i].m_blend_enable = true;
					l_config.m_render_target[i].m_src_blend = graphic::bc_blend::src_alpha;
					l_config.m_render_target[i].m_dest_blend = graphic::bc_blend::inv_src_alpha;
					l_config.m_render_target[i].m_blend_op = graphic::bc_blend_op::add;
					l_config.m_render_target[i].m_src_blend_alpha = graphic::bc_blend::one;
					l_config.m_render_target[i].m_dest_blend_alpha = graphic::bc_blend::zero;
					l_config.m_render_target[i].m_blend_op_alpha = graphic::bc_blend_op::add;
					l_config.m_render_target[i].m_render_target_write_mask = static_cast<bcUINT8>(graphic::bc_color_write::all);
				}
				break;
			}
			case bc_blend_type::blending_preserve_alpha:
			{
				l_config.m_alpha_to_coverage_enable = false;
				l_config.m_independent_blend_enable = false;
				for (bcUINT32 i = 0; i < graphic::bc_render_api_info::number_of_om_render_target_slots(); i++)
				{
					l_config.m_render_target[i].m_blend_enable = true;
					l_config.m_render_target[i].m_src_blend = graphic::bc_blend::src_alpha;
					l_config.m_render_target[i].m_dest_blend = graphic::bc_blend::inv_src_alpha;
					l_config.m_render_target[i].m_blend_op = graphic::bc_blend_op::add;
					l_config.m_render_target[i].m_src_blend_alpha = graphic::bc_blend::zero;
					l_config.m_render_target[i].m_dest_blend_alpha = graphic::bc_blend::one;
					l_config.m_render_target[i].m_blend_op_alpha = graphic::bc_blend_op::add;
					l_config.m_render_target[i].m_render_target_write_mask = static_cast<bcUINT8>(graphic::bc_color_write::all);
				}
				break;
			}
			case bc_blend_type::additive_overwrite_alpha:
			{
				l_config.m_alpha_to_coverage_enable = false;
				l_config.m_independent_blend_enable = false;
				for (bcUINT32 i = 0; i < graphic::bc_render_api_info::number_of_om_render_target_slots(); i++)
				{
					l_config.m_render_target[i].m_blend_enable = true;
					l_config.m_render_target[i].m_src_blend = graphic::bc_blend::src_alpha;
					l_config.m_render_target[i].m_dest_blend = graphic::bc_blend::one;
					l_config.m_render_target[i].m_blend_op = graphic::bc_blend_op::add;
					l_config.m_render_target[i].m_src_blend_alpha = graphic::bc_blend::one;
					l_config.m_render_target[i].m_dest_blend_alpha = graphic::bc_blend::zero;
					l_config.m_render_target[i].m_blend_op_alpha = graphic::bc_blend_op::add;
					l_config.m_render_target[i].m_render_target_write_mask = static_cast<bcUINT8>(graphic::bc_color_write::all);
				}
				break;
			}
			case bc_blend_type::additive_preserve_alpha:
			{
				l_config.m_alpha_to_coverage_enable = false;
				l_config.m_independent_blend_enable = false;
				for (bcUINT32 i = 0; i < graphic::bc_render_api_info::number_of_om_render_target_slots(); i++)
				{
					l_config.m_render_target[i].m_blend_enable = true;
					l_config.m_render_target[i].m_src_blend = graphic::bc_blend::src_alpha;
					l_config.m_render_target[i].m_dest_blend = graphic::bc_blend::one;
					l_config.m_render_target[i].m_blend_op = graphic::bc_blend_op::add;
					l_config.m_render_target[i].m_src_blend_alpha = graphic::bc_blend::zero;
					l_config.m_render_target[i].m_dest_blend_alpha = graphic::bc_blend::one;
					l_config.m_render_target[i].m_blend_op_alpha = graphic::bc_blend_op::add;
					l_config.m_render_target[i].m_render_target_write_mask = static_cast<bcUINT8>(graphic::bc_color_write::all);
				}
				break;
			}
			default:
				throw bc_invalid_argument_exception("Invalid blend type");
			}

			return l_config;
		}

		graphic::bc_depth_stencil_state_config bc_graphic_state_configs::bc_depth_stencil_config(bc_depth_stencil_type p_value)
		{
			graphic::bc_depth_stencil_state_config l_config;

			auto l_depth_config = bc_depth_stencil_type::depth_off;
			auto l_stencil_config = bc_depth_stencil_type::stencil_off;

			if(core::bc_enum::has(p_value, bc_depth_stencil_type::depth_less))
			{
				l_depth_config = bc_depth_stencil_type::depth_less;
			}
			if(core::bc_enum::has(p_value, bc_depth_stencil_type::depth_less_no_write))
			{
				l_depth_config = bc_depth_stencil_type::depth_less_no_write;
			}
			if (core::bc_enum::has(p_value, bc_depth_stencil_type::stencil_write))
			{
				l_stencil_config = bc_depth_stencil_type::stencil_write;
			}

			switch (l_depth_config)
			{
			case bc_depth_stencil_type::depth_off:
			{
				l_config.m_depth_enable = false;
				l_config.m_depth_write_mask = graphic::bc_depth_write_mask::zero;
				l_config.m_depth_func = graphic::bc_comparison_func::never;
				break;
			}
			case bc_depth_stencil_type::depth_less:
			{
				l_config.m_depth_enable = true;
				l_config.m_depth_write_mask = graphic::bc_depth_write_mask::all;
				l_config.m_depth_func = graphic::bc_comparison_func::less;
				break;
			}
			case bc_depth_stencil_type::depth_less_no_write:
			{
				l_config.m_depth_enable = true;
				l_config.m_depth_write_mask = graphic::bc_depth_write_mask::zero;
				l_config.m_depth_func = graphic::bc_comparison_func::less;
				break;
			}
			default: ;
			}

			switch (l_stencil_config)
			{
			case bc_depth_stencil_type::stencil_off:
			{
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
			case bc_depth_stencil_type::stencil_write:
			{
				l_config.m_stencil_enable = true;
				l_config.m_stencil_read_mask = 0xff;
				l_config.m_stencil_write_mask = 0xff;

				l_config.m_front_face.m_stencil_func = graphic::bc_comparison_func::always;
				l_config.m_front_face.m_stencil_pass_op = graphic::bc_stencil_op::replace;
				l_config.m_front_face.m_stencil_fail_op = graphic::bc_stencil_op::replace;
				l_config.m_front_face.m_stencil_depth_fail_op = graphic::bc_stencil_op::keep;

				l_config.m_back_face.m_stencil_func = graphic::bc_comparison_func::always;
				l_config.m_back_face.m_stencil_pass_op = graphic::bc_stencil_op::replace;
				l_config.m_back_face.m_stencil_fail_op = graphic::bc_stencil_op::replace;
				l_config.m_back_face.m_stencil_depth_fail_op = graphic::bc_stencil_op::keep;
				break;
			}
			default: ;
			}

			return l_config;
		}

		graphic::bc_rasterizer_state_config bc_graphic_state_configs::bc_rasterizer_config(bc_rasterizer_type p_value)
		{
			graphic::bc_rasterizer_state_config l_config = graphic::bc_rasterizer_state_config::default_config();

			switch (p_value)
			{
			case bc_rasterizer_type::fill_solid_cull_back:
				l_config.m_fill_mode = graphic::bc_fill_mode::solid;
				l_config.m_cull_mode = graphic::bc_cull_mode::back;
				break;

			case bc_rasterizer_type::fill_solid_cull_front:
				l_config.m_fill_mode = graphic::bc_fill_mode::solid;
				l_config.m_cull_mode = graphic::bc_cull_mode::front;
				break;

			case bc_rasterizer_type::fill_solid_cull_none:
				l_config.m_fill_mode = graphic::bc_fill_mode::solid;
				l_config.m_cull_mode = graphic::bc_cull_mode::none;
				break;

			case bc_rasterizer_type::fill_wireframe_cull_back:
				l_config.m_fill_mode = graphic::bc_fill_mode::wireframe;
				l_config.m_cull_mode = graphic::bc_cull_mode::back;
				break;

			case bc_rasterizer_type::fill_wireframe_cull_front:
				l_config.m_fill_mode = graphic::bc_fill_mode::wireframe;
				l_config.m_cull_mode = graphic::bc_cull_mode::front;
				break;

			case bc_rasterizer_type::fill_wireframe_cull_none:
				l_config.m_fill_mode = graphic::bc_fill_mode::wireframe;
				l_config.m_cull_mode = graphic::bc_cull_mode::none;
				break;

			default:
				throw bc_invalid_argument_exception("Invalid rasterizer type");
			}

			return l_config;
		}

		graphic::bc_input_layout_config bc_graphic_state_configs::bc_input_layout_config(bc_vertex_type p_value)
		{
			graphic::bc_input_layout_config l_config;

			switch (p_value)
			{
			case bc_vertex_type::none:
				break;
			case bc_vertex_type::pos:
				l_config.m_input_elements.assign
				(
					{
						graphic::bc_input_element_desc
						{
							"POSITION", 0, graphic::bc_format::R32G32B32_FLOAT, 0, 0, graphic::bc_input_classification::per_vertex_data, 0
						}
					}
				);
				break;

			case bc_vertex_type::pos_tex:
				l_config.m_input_elements.assign
				(
					{
						graphic::bc_input_element_desc
						{
							"POSITION", 0, graphic::bc_format::R32G32B32_FLOAT, 0, 0, graphic::bc_input_classification::per_vertex_data, 0
						},
						graphic::bc_input_element_desc
						{
							"TEXCOORD", 0, graphic::bc_format::R32G32_FLOAT, 0, 12, graphic::bc_input_classification::per_vertex_data, 0
						}
					}
				);
				break;

			case bc_vertex_type::pos_tex_nor:
				l_config.m_input_elements.assign
				(
					{
						graphic::bc_input_element_desc
						{
							"POSITION", 0, graphic::bc_format::R32G32B32_FLOAT, 0, 0, graphic::bc_input_classification::per_vertex_data, 0
						},
						graphic::bc_input_element_desc
						{
							"TEXCOORD", 0, graphic::bc_format::R32G32_FLOAT, 0, 12, graphic::bc_input_classification::per_vertex_data, 0
						},
						graphic::bc_input_element_desc
						{
							"NORMAL", 0, graphic::bc_format::R32G32B32_FLOAT, 0, 20, graphic::bc_input_classification::per_vertex_data, 0
						},
					}
				);
				break;
			case bc_vertex_type::pos_tex_nor_tan:
				l_config.m_input_elements.assign
				(
					{
						graphic::bc_input_element_desc
						{
							"POSITION", 0, graphic::bc_format::R32G32B32_FLOAT, 0, 0, graphic::bc_input_classification::per_vertex_data, 0
						},
						graphic::bc_input_element_desc
						{
							"TEXCOORD", 0, graphic::bc_format::R32G32_FLOAT, 0, 12, graphic::bc_input_classification::per_vertex_data, 0
						},
						graphic::bc_input_element_desc
						{
							"NORMAL", 0, graphic::bc_format::R32G32B32_FLOAT, 0, 20, graphic::bc_input_classification::per_vertex_data, 0
						},
						graphic::bc_input_element_desc
						{
							"TANGENT", 0, graphic::bc_format::R32G32B32_FLOAT, 0, 32, graphic::bc_input_classification::per_vertex_data, 0
						},
					}
				);
				break;
			case bc_vertex_type::pos_tex_nor_tan_bon:
				l_config.m_input_elements.assign
				(
					{
						graphic::bc_input_element_desc
						{
							"POSITION", 0, graphic::bc_format::R32G32B32_FLOAT, 0, 0, graphic::bc_input_classification::per_vertex_data, 0
						},
						graphic::bc_input_element_desc
						{
							"TEXCOORD", 0, graphic::bc_format::R32G32_FLOAT, 0, 12, graphic::bc_input_classification::per_vertex_data, 0
						},
						graphic::bc_input_element_desc
						{
							"NORMAL", 0, graphic::bc_format::R32G32B32_FLOAT, 0, 20, graphic::bc_input_classification::per_vertex_data, 0
						},
						graphic::bc_input_element_desc
						{
							"TANGENT", 0, graphic::bc_format::R32G32B32_FLOAT, 0, 32, graphic::bc_input_classification::per_vertex_data, 0
						},
						graphic::bc_input_element_desc
						{
							"BLENDINDICES", 0, graphic::bc_format::R32G32B32A32_SINT, 0, 44, graphic::bc_input_classification::per_vertex_data, 0
						},
						graphic::bc_input_element_desc
						{
							"BLENDWEIGHT", 0, graphic::bc_format::R32G32B32A32_FLOAT, 0, 60, graphic::bc_input_classification::per_vertex_data, 0
						},
					}
				);
				break;

			default:
				throw bc_invalid_argument_exception("Invalid vertex type");
			}

			return l_config;
		}

		graphic::bc_texture_ms_config bc_graphic_state_configs::bc_texture_multi_sample(bc_multi_sample_type p_ms)
		{
			graphic::bc_texture_ms_config l_config(1, 1);

			switch (p_ms)
			{
			case bc_multi_sample_type::c1_q1:
				l_config = graphic::bc_texture_ms_config(1, 1);
				break;

			default:
				throw bc_invalid_argument_exception("Invalid multi-sample type");
			}

			return l_config;
		}
	}
}