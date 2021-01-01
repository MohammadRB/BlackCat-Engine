// [03/27/2019 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Container/bcArray.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/System/Render/bcFrameRenderer.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "BlackCat/RenderPass/bcBackBufferWritePass.h"
#include "BlackCat/bcConstant.h"

namespace black_cat
{
	bc_back_buffer_write_pass::bc_back_buffer_write_pass(constant::bc_render_pass_variable_t p_input_texture)
		: m_input_texture(p_input_texture)
	{
	}
	
	void bc_back_buffer_write_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
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

		core::bc_array<game::bc_vertex_pos_tex, 4> l_vertices
		{
			game::bc_vertex_pos_tex{ core::bc_vector3f(-1, 1, 1), core::bc_vector2f(0, 0) },
			game::bc_vertex_pos_tex{ core::bc_vector3f(1, 1, 1), core::bc_vector2f(1, 0) },
			game::bc_vertex_pos_tex{ core::bc_vector3f(-1, -1, 1), core::bc_vector2f(0, 1) },
			game::bc_vertex_pos_tex{ core::bc_vector3f(1, -1, 1), core::bc_vector2f(1, 1) },
		};
		core::bc_array<bcUINT16, 6> l_indices
		{
			0, 1, 2, 1, 3, 2
		};

		auto l_vertices_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(6, sizeof(game::bc_vertex_pos_tex), graphic::bc_resource_usage::gpu_r)
			.as_vertex_buffer();
		auto l_indices_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(6, sizeof(game::bc_vertex_pos_tex), graphic::bc_resource_usage::gpu_r)
			.as_index_buffer();
		auto l_vertices_buffer_data = graphic::bc_subresource_data(l_vertices.data(), 0, 0);
		auto l_indices_buffer_data = graphic::bc_subresource_data(l_indices.data(), 0, 0);
		m_vertices_buffer = l_device.create_buffer(l_vertices_buffer_config, &l_vertices_buffer_data);
		m_indices_buffer = l_device.create_buffer(l_indices_buffer_config, &l_indices_buffer_data);

		m_render_state = p_render_system.create_render_state
		(
			graphic::bc_primitive::trianglelist,
			m_vertices_buffer.get(),
			sizeof(game::bc_vertex_pos_tex),
			0,
			m_indices_buffer.get(),
			game::i16bit,
			6,
			0,
			{},
			{}
		);

		graphic::bc_device_parameters l_old_parameters
		(
			0,
			0,
			graphic::bc_format::unknown,
			graphic::bc_texture_ms_config(1, 0)
		);
		graphic::bc_device_parameters l_new_parameters
		(
			l_device.get_back_buffer_width(),
			l_device.get_back_buffer_height(),
			l_device.get_back_buffer_format(),
			l_device.get_back_buffer_texture().get_sample_count()
		);

		after_reset(game::bc_render_pass_reset_context(p_render_system, l_device, l_old_parameters, l_new_parameters));
	}

	void bc_back_buffer_write_pass::update(const game::bc_render_pass_update_context& p_param)
	{
	}

	void bc_back_buffer_write_pass::initialize_frame(const game::bc_render_pass_render_context& p_param)
	{
	}
	
	void bc_back_buffer_write_pass::execute(const game::bc_render_pass_render_context& p_param)
	{
		p_param.m_render_thread.start();
		p_param.m_render_thread.bind_render_pass_state(*m_render_pass_state);
		p_param.m_render_thread.bind_render_state(*m_render_state);

		p_param.m_render_thread.draw_indexed(0, 6, 0);

		p_param.m_render_thread.unbind_render_state(*m_render_state);
		p_param.m_render_thread.unbind_render_pass_state(*m_render_pass_state);
		p_param.m_render_thread.finish();
	}

	void bc_back_buffer_write_pass::before_reset(const game::bc_render_pass_reset_context& p_param)
	{
		// Release references to back-buffer
		m_render_pass_state.reset();
		m_back_buffer_view.reset();
	}

	void bc_back_buffer_write_pass::after_reset(const game::bc_render_pass_reset_context& p_param)
	{
		if
		(
			p_param.m_old_parameters.m_width == p_param.m_new_parameters.m_width
			&&
			p_param.m_old_parameters.m_height == p_param.m_new_parameters.m_height
		)
		{
			return;
		}

		auto l_resource_configure = graphic::bc_graphic_resource_builder();

		auto l_input_texture = *get_shared_resource<graphic::bc_texture2d>(m_input_texture);
		auto l_back_buffer = p_param.m_device.get_back_buffer_texture();

		auto l_input_texture_view_config = l_resource_configure
			.as_resource_view()
			.as_texture_view(l_input_texture.get_format())
			.as_tex2d_shader_view(0, 1)
			.on_texture2d();
		auto l_back_buffer_view_config = l_resource_configure
			.as_resource_view()
			.as_texture_view(l_back_buffer.get_format())
			.as_tex2d_render_target_view(0);

		m_input_texture_view = p_param.m_device.create_resource_view(l_input_texture, l_input_texture_view_config);
		m_back_buffer_view = p_param.m_device.create_render_target_view(l_back_buffer, l_back_buffer_view_config);
		const auto l_viewport = graphic::bc_viewport::default_config(l_back_buffer.get_width(), l_back_buffer.get_height());

		m_pipeline_state = p_param.m_render_system.create_device_pipeline_state
		(
			"bb_write_vs",
			nullptr,
			nullptr,
			nullptr,
			"bb_write_ps",
			game::bc_vertex_type::pos_tex,
			game::bc_blend_type::opaque,
			game::bc_depth_stencil_type::depth_off_stencil_off,
			game::bc_rasterizer_type::fill_solid_cull_none,
			0x1,
			{ p_param.m_device.get_back_buffer_format() },
			get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_depth_stencil_texture)->get_format(),
			game::bc_multi_sample_type::c1_q1
		);
		m_render_pass_state = p_param.m_render_system.create_render_pass_state
		(
			m_pipeline_state.get(),
			l_viewport,
			{ m_back_buffer_view.get() },
			graphic::bc_depth_stencil_view(),
			{ graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_sampler_state.get()) },
			{ graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, m_input_texture_view.get()) },
			{},
			{
				p_param.m_render_system.get_global_cbuffer()
			}
		);

		share_resource(constant::g_rpass_back_buffer_view, m_back_buffer_view.get());
	}

	void bc_back_buffer_write_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_render_state.reset();
		m_render_pass_state.reset();
		m_pipeline_state.reset();

		m_input_texture_view.reset();
		m_back_buffer_view.reset();
		m_sampler_state.reset();
		
		m_vertices_buffer.reset();
		m_indices_buffer.reset();
	}
}