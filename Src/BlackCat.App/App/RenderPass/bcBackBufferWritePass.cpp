// [27/03/2019 MRB]

#include "App/AppPCH.h"

#include "Core/Container/bcArray.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/System/Render/bcFrameRenderer.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "App/RenderPass/bcBackBufferWritePass.h"
#include "App/bcConstant.h"

namespace black_cat
{
	bc_back_buffer_write_pass::bc_back_buffer_write_pass(game::bc_render_pass_variable_t p_input_texture, game::bc_render_pass_variable_t p_input_texture_view)
		: m_input_texture(p_input_texture),
		m_input_texture_view(p_input_texture_view)
	{
	}

	void bc_back_buffer_write_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		auto& l_device_swap_buffer = p_render_system.get_device_swap_buffer();
		
		const auto l_sampler_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_sampler_state
			(
				graphic::bc_filter::min_mag_mip_point,
				graphic::bc_texture_address_mode::wrap,
				graphic::bc_texture_address_mode::wrap,
				graphic::bc_texture_address_mode::wrap
			).as_sampler_state();
		m_sampler_state = l_device.create_sampler_state(l_sampler_config);
		
		m_render_state = p_render_system.create_render_state
		(
			graphic::bc_primitive::trianglelist,
			graphic::bc_buffer(),
			sizeof(game::bc_vertex_pos_tex),
			0,
			graphic::bc_buffer(),
			game::i16bit,
			0,
			0,
			{},
			{}
		);

		after_reset(game::bc_render_pass_reset_context::create_default_instance(p_render_system, l_device, l_device_swap_buffer));
	}

	void bc_back_buffer_write_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_back_buffer_write_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
	}
	
	void bc_back_buffer_write_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
		p_context.m_render_thread.start();
		p_context.m_render_thread.bind_render_pass_state(*m_render_pass_state);
		p_context.m_render_thread.bind_render_state(*m_render_state);

		p_context.m_render_thread.draw(0, 6);

		p_context.m_render_thread.unbind_render_state(*m_render_state);
		p_context.m_render_thread.unbind_render_pass_state(*m_render_pass_state);
		p_context.m_render_thread.finish();
	}

	void bc_back_buffer_write_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
		// Release references to back-buffer
		m_render_pass_state.reset();
	}

	void bc_back_buffer_write_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		const auto l_back_buffer_texture = get_shared_resource_throw<graphic::bc_texture2d>(constant::g_rpass_back_buffer_texture);
		const auto l_back_buffer_render_view = get_shared_resource_throw<graphic::bc_render_target_view>(constant::g_rpass_back_buffer_render_view);
		const auto l_input_texture = get_shared_resource_throw<graphic::bc_texture2d>(m_input_texture);
		const auto l_input_texture_view = get_shared_resource_throw<graphic::bc_resource_view>(m_input_texture_view);
		const auto l_viewport = graphic::bc_viewport::default_config(l_back_buffer_texture.get_width(), l_back_buffer_texture.get_height());
		
		m_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"bb_write_vs",
			nullptr,
			nullptr,
			nullptr,
			"bb_write_ps",
			game::bc_vertex_type::pos_tex,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0x1,
			{ p_context.m_device_swap_buffer.get_back_buffer_format() },
			get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_depth_stencil_texture)->get_format(),
			game::bc_multi_sample_type::c1_q1
		);
		m_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			m_pipeline_state.get(),
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(l_back_buffer_render_view)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_sampler_state.get())
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_input_texture_view)
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer()
			}
		);
	}

	void bc_back_buffer_write_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_render_state.reset();
		m_render_pass_state.reset();
		m_pipeline_state.reset();
		
		m_sampler_state.reset();
	}
}