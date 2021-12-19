// [12/17/2021 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "BlackCat/RenderPass/PostProcess/bcGlowPass.h"
#include "BlackCat/bcConstant.h"

namespace black_cat
{
	struct _bc_quad_vertex
	{
		core::bc_vector2f m_vertex;
		core::bc_vector2f m_texcoord;
	};

	struct _bc_parameters_struct
	{
		BC_CBUFFER_ALIGN
		bcFLOAT m_threshold;
		bcFLOAT m_intensity;
	};

	bc_glow_pass::bc_glow_pass(game::bc_render_pass_variable_t p_render_target_texture, game::bc_render_pass_variable_t p_render_target_view):
		m_render_target_texture(p_render_target_texture),
		m_render_target_view(p_render_target_view),
		m_intermediate_texture_config()
	{
	}

	void bc_glow_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		auto& l_device_swap_buffer = p_render_system.get_device_swap_buffer();

		const auto l_point_sampler_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_sampler_state
			(
				graphic::bc_filter::min_mag_mip_point,
				graphic::bc_texture_address_mode::clamp,
				graphic::bc_texture_address_mode::clamp,
				graphic::bc_texture_address_mode::clamp
			)
			.as_sampler_state();
		const auto l_linear_sampler_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_sampler_state
			(
				graphic::bc_filter::min_mag_mip_linear,
				graphic::bc_texture_address_mode::clamp,
				graphic::bc_texture_address_mode::clamp,
				graphic::bc_texture_address_mode::clamp
			)
			.as_sampler_state();
		m_point_sampler = l_device.create_sampler_state(l_point_sampler_config);
		m_linear_sampler = l_device.create_sampler_state(l_linear_sampler_config);

		const auto l_parameters_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(1, sizeof(_bc_parameters_struct), graphic::bc_resource_usage::gpu_rw)
			.as_constant_buffer();
		auto l_parameters_struct = _bc_parameters_struct{.8f, .5f};
		const graphic::bc_subresource_data l_parameters_data{ &l_parameters_struct, 0,0 };
		m_parameters_buffer = l_device.create_buffer(l_parameters_buffer_config, &l_parameters_data);

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
		if(!m_enabled)
		{
			return;
		}

		{
			const auto l_intermediate_texture1 = get_intermediate_texture(m_intermediate_texture_config);
			const auto l_intermediate_texture2 = get_intermediate_texture(m_intermediate_texture_config);

			p_context.m_render_thread.start();

			// Glow extract
			m_intermediate_texture1_link.set_as_render_target_view(l_intermediate_texture1.get_render_target_view());

			p_context.m_render_thread.bind_render_pass_state(*m_glow_extract_render_pass_state);
			p_context.m_render_thread.bind_render_state(*m_glow_extract_render_state);
			p_context.m_render_thread.draw(0, 6);
			p_context.m_render_thread.unbind_render_state(*m_glow_extract_render_state);
			p_context.m_render_thread.unbind_render_pass_state(*m_glow_extract_render_pass_state);

			// Blur
			m_intermediate_texture1_link.set_as_resource_view(l_intermediate_texture1.get_resource_view());
			m_intermediate_texture2_link.set_as_render_target_view(l_intermediate_texture2.get_render_target_view());

			p_context.m_render_thread.bind_render_pass_state(*m_blur_render_pass_state);
			p_context.m_render_thread.bind_render_state(*m_blur_render_state);
			p_context.m_render_thread.draw(0, 6);
			p_context.m_render_thread.unbind_render_state(*m_blur_render_state);
			p_context.m_render_thread.unbind_render_pass_state(*m_blur_render_pass_state);

			// Apply
			m_intermediate_texture2_link.set_as_resource_view(l_intermediate_texture2.get_resource_view());

			p_context.m_render_thread.bind_render_pass_state(*m_glow_apply_render_pass_state);
			p_context.m_render_thread.bind_render_state(*m_glow_apply_render_state);
			p_context.m_render_thread.draw(0, 6);
			p_context.m_render_thread.unbind_render_state(*m_glow_apply_render_state);
			p_context.m_render_thread.unbind_render_pass_state(*m_glow_apply_render_pass_state);

			p_context.m_render_thread.finish();
		}
	}

	void bc_glow_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
		if
		(
			p_context.m_old_parameters.m_width == p_context.m_new_parameters.m_width &&
			p_context.m_old_parameters.m_height == p_context.m_new_parameters.m_height
		)
		{
			return;
		}

		// Release reference to back buffer before reset
		m_render_target_resource_view.reset();
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

		const auto l_render_target_texture = get_shared_resource_throw<graphic::bc_texture2d>(m_render_target_texture);
		const auto l_render_target_view = get_shared_resource_throw<graphic::bc_render_target_view>(m_render_target_view);
		const auto l_half_viewport = graphic::bc_viewport::default_config(l_render_target_texture.get_width() / 2, l_render_target_texture.get_height() / 2);
		const auto l_full_viewport = graphic::bc_viewport::default_config(l_render_target_texture.get_width(), l_render_target_texture.get_height());

		m_intermediate_texture_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_texture2d
			(
				l_render_target_texture.get_width() / 2,
				l_render_target_texture.get_height() / 2,
				false,
				1,
				l_render_target_texture.get_format(),
				graphic::bc_resource_usage::gpu_rw,
				core::bc_enum::mask_or
				({ 
					graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::render_target
				})
			)
			.as_render_target_texture();

		const auto l_render_target_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_texture_view(l_render_target_texture.get_format())
			.as_tex2d_shader_view(0, 1)
			.on_render_target_texture2d();
		m_render_target_resource_view = p_context.m_device.create_resource_view(l_render_target_texture, l_render_target_view_config);

		m_glow_extract_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"glow_extract_vs",
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
				l_render_target_texture.get_format()
			},
			graphic::bc_format::D24_UNORM_S8_UINT,
			game::bc_multi_sample_type::c1_q1
		);
		m_glow_extract_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			*m_glow_extract_device_pipeline_state,
			l_half_viewport,
			{
				graphic::bc_render_target_view_parameter(m_intermediate_texture1_link)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, *m_point_sampler)
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, *m_render_target_resource_view)
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::pixel, *m_parameters_buffer)
			}
		);
		m_glow_extract_render_state = p_context.m_render_system.create_render_state
		(
			graphic::bc_primitive::trianglelist,
			graphic::bc_buffer(),
			0,
			0,
			graphic::bc_buffer(),
			game::i16bit,
			0,
			0,
			{
			},
			{
			}
		);

		m_blur_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"glow_extract_vs",
			nullptr,
			nullptr,
			nullptr,
			"gaussian_blur_ps",
			game::bc_vertex_type::pos_tex,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0xffffffff,
			{
				l_render_target_texture.get_format()
			},
			graphic::bc_format::D24_UNORM_S8_UINT,
			game::bc_multi_sample_type::c1_q1
		);
		m_blur_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			*m_blur_device_pipeline_state,
			l_half_viewport,
			{
				graphic::bc_render_target_view_parameter(m_intermediate_texture2_link)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, *m_point_sampler)
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, m_intermediate_texture1_link)
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer()
			}
		);
		m_blur_render_state = p_context.m_render_system.create_render_state
		(
			graphic::bc_primitive::trianglelist,
			graphic::bc_buffer(),
			0,
			0,
			graphic::bc_buffer(),
			game::i16bit,
			0,
			0,
			{
			},
			{
			}
		);

		m_glow_apply_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"glow_extract_vs",
			nullptr,
			nullptr,
			nullptr,
			"glow_apply_ps",
			game::bc_vertex_type::pos_tex,
			game::bc_blend_type::additive_preserve_alpha,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0xffffffff,
			{
				l_render_target_texture.get_format()
			},
			graphic::bc_format::D24_UNORM_S8_UINT,
			game::bc_multi_sample_type::c1_q1
		);
		m_glow_apply_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			*m_glow_apply_device_pipeline_state,
			l_full_viewport,
			{
				graphic::bc_render_target_view_parameter(l_render_target_view)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, *m_linear_sampler)
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, m_intermediate_texture2_link)
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer()
			}
		);
		m_glow_apply_render_state = p_context.m_render_system.create_render_state
		(
			graphic::bc_primitive::trianglelist,
			graphic::bc_buffer(),
			0,
			0,
			graphic::bc_buffer(),
			game::i16bit,
			0,
			0,
			{
			},
			{
			}
		);
	}

	void bc_glow_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_glow_extract_device_pipeline_state.reset();
		m_glow_extract_render_pass_state.reset();
		m_glow_extract_render_state.reset();
		m_blur_device_pipeline_state.reset();
		m_blur_render_pass_state.reset();
		m_blur_render_state.reset();
		m_glow_apply_device_pipeline_state.reset();
		m_glow_apply_render_pass_state.reset();
		m_glow_apply_render_state.reset();

		m_point_sampler.reset();
		m_linear_sampler.reset();
		m_render_target_resource_view.reset();
		m_render_target_resource_view.reset();
	}

	void bc_glow_pass::update_parameters(game::bc_default_render_thread& p_render_thread, bcFLOAT p_threshold, bcFLOAT p_intensity) noexcept
	{
		const _bc_parameters_struct l_parameters
		{
			p_threshold, p_intensity
		};

		p_render_thread.update_subresource(*m_parameters_buffer, 0, &l_parameters, 0, 0);
	}
}