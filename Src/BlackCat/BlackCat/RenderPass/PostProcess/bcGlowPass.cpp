// [12/17/2021 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "BlackCat/RenderPass/PostProcess/bcGlowPass.h"

namespace black_cat
{
	void bc_glow_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		auto& l_device_swap_buffer = p_render_system.get_device_swap_buffer();

		after_reset
		(
			game::bc_render_pass_reset_context
			(
				p_render_system,
				l_device,
				l_device_swap_buffer,
				graphic::bc_device_parameters
				(
					0,
					0,
					graphic::bc_format::unknown,
					graphic::bc_texture_ms_config(1, 0)
				),
				graphic::bc_device_parameters
				(
					l_device_swap_buffer.get_back_buffer_width(),
					l_device_swap_buffer.get_back_buffer_height(),
					l_device_swap_buffer.get_back_buffer_format(),
					l_device_swap_buffer.get_back_buffer_texture().get_sample_count()
				)
			)
		);
	}

	void bc_glow_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_glow_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
	}

	void bc_glow_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
	}

	void bc_glow_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_glow_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		if
		(
			p_context.m_old_parameters.m_width == p_context.m_new_parameters.m_width &&
			p_context.m_old_parameters.m_height == p_context.m_new_parameters.m_height
		)
		{
			return;
		}

		const auto l_back_buffer = p_context.m_device_swap_buffer.get_back_buffer_texture();
		const auto l_viewport = graphic::bc_viewport::default_config(l_back_buffer.get_width(), l_back_buffer.get_height());

		const auto l_back_buffer_read_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_texture_view(l_back_buffer.get_format())
			.as_tex2d_shader_view(0, 1)
			.on_render_target_texture2d();
		const auto l_back_buffer_read_view = p_context.m_device.create_resource_view(l_back_buffer, l_back_buffer_read_view_config);

		m_glow_extract_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"vs",
			nullptr,
			nullptr,
			nullptr,
			"glow_extract_ps",
			game::bc_vertex_type::pos_tex,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off}),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0xffffffff,
			{
				l_back_buffer.get_format()
			},
			graphic::bc_format::D24_UNORM_S8_UINT,
			game::bc_multi_sample_type::c1_q1
		);
		/*m_glow_extract_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			*m_glow_extract_device_pipeline_state,
			l_viewport,
			{
				
			}
		);*/
	}

	void bc_glow_pass::destroy(game::bc_render_system& p_render_system)
	{
	}
}