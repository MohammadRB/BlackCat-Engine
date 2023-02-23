// [12/17/2021 MRB]

#include "App/AppPCH.h"

#include "Core/Container/bcArray.h"
#include "Core/Container/bcStringStream.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "App/RenderPass/PostProcess/bcGlowPass.h"
#include "App/bcConstant.h"

namespace black_cat
{
	struct _bc_quad_vertex
	{
		core::bc_vector2f m_vertex;
		core::bc_vector2f m_texcoord;
	};

	struct _bc_glow_params_struct
	{
		BC_CBUFFER_ALIGN
		bcFLOAT m_threshold;
		bcFLOAT m_intensity;
		bcFLOAT m_apply_multiplier;
	};

	struct _bc_blur_params_struct
	{
		BC_CBUFFER_ALIGN
		bcUINT32 m_width;
		bcUINT32 m_height;
	};

	bc_glow_pass::bc_glow_pass(game::bc_render_pass_variable_t p_render_target_texture, game::bc_render_pass_variable_t p_render_target_view)
		: m_render_target_texture_param(p_render_target_texture),
		  m_render_target_view_param(p_render_target_view),
		  m_back_buffer_width(0),
		  m_back_buffer_height(0),
		  m_glow_intermediate_texture_config(),
		  m_upscale_intermediate_texture_config()
	{
		auto& l_global_config = bc_get_global_config();
		l_global_config
				.add_if_not_exist_config_key("render_glow_threshold", core::bc_any(.88f))
				.add_if_not_exist_config_key("render_glow_intensity", core::bc_any(5.f))
				.flush_changes();
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

		const auto l_glow_params_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(1, sizeof(_bc_glow_params_struct), graphic::bc_resource_usage::gpu_rw)
			.as_constant_buffer();
		m_glow_params_buffer = l_device.create_buffer(l_glow_params_buffer_config, nullptr);
		m_glow_params_buffer->set_debug_name("glow_params_cbuffer");

		const auto l_blur_params_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(1, sizeof(_bc_blur_params_struct), graphic::bc_resource_usage::gpu_rw)
			.as_constant_buffer();
		m_blur_params_buffer = l_device.create_buffer(l_blur_params_buffer_config, nullptr);
		m_blur_params_buffer->set_debug_name("blur_params_cbuffer");

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
		if (m_glow_intensity <= 0)
		{
			return;
		}

		p_context.m_render_thread.start();

		if (m_parameters_changed)
		{
			_update_parameters(p_context.m_render_thread);
			m_parameters_changed = false;
		}

		// Glow extract
		const auto l_glow_intermediate_texture = get_intermediate_texture(m_glow_intermediate_texture_config);
		m_glow_texture_link.set_as_render_target_view(l_glow_intermediate_texture.get_render_target_view());

		p_context.m_render_thread.bind_render_pass_state(*m_glow_extract_render_pass_state);
		p_context.m_render_thread.bind_render_state(*m_glow_extract_render_state);
		p_context.m_render_thread.draw(0, 6);
		p_context.m_render_thread.unbind_render_state(*m_glow_extract_render_state);
		p_context.m_render_thread.unbind_render_pass_state(*m_glow_extract_render_pass_state);

		core::bc_vector_frame<game::bc_intermediate_texture_guard> l_blur_textures;
		l_blur_textures.reserve(m_blur_steps.size());

		// Blur steps
		for (auto& l_step : m_blur_steps)
		{
			auto l_blur_intermediate_texture_config = graphic::bc_graphic_resource_builder()
				.as_resource()
				.as_texture2d
				(
					m_back_buffer_width / l_step.m_size,
					m_back_buffer_height / l_step.m_size,
					false,
					1,
					graphic::bc_format::R8G8B8A8_UNORM,
					graphic::bc_resource_usage::gpu_rw,
					core::bc_enum::mask_or
					({
						graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::render_target
					})
				)
				.as_render_target_texture();
			const auto l_intermediate_texture = get_intermediate_texture(l_blur_intermediate_texture_config);
			auto l_result_intermediate_texture = get_intermediate_texture(l_blur_intermediate_texture_config);

			const _bc_blur_params_struct l_blur_params
			{
				l_blur_intermediate_texture_config.get_width(),
				l_blur_intermediate_texture_config.get_height()
			};
			p_context.m_render_thread.update_subresource(*m_blur_params_buffer, 0, &l_blur_params, 0, 0);

			// Blur horizontal
			if (l_blur_textures.empty())
			{
				m_glow_texture_link.set_as_resource_view(l_glow_intermediate_texture.get_resource_view());
			}
			else
			{
				m_glow_texture_link.set_as_resource_view(l_blur_textures.back().get_resource_view());
			}

			l_step.m_intermediate_texture_link.set_as_render_target_view(l_intermediate_texture.get_render_target_view());

			p_context.m_render_thread.bind_render_pass_state(*l_step.m_hor_blur_render_pass_state);
			p_context.m_render_thread.bind_render_state(*l_step.m_hor_blur_render_state);
			p_context.m_render_thread.draw(0, 6);
			p_context.m_render_thread.unbind_render_state(*l_step.m_hor_blur_render_state);
			p_context.m_render_thread.unbind_render_pass_state(*l_step.m_hor_blur_render_pass_state);

			// Blur vertical
			l_step.m_intermediate_texture_link.set_as_resource_view(l_intermediate_texture.get_resource_view());
			l_step.m_result_texture_link.set_as_render_target_view(l_result_intermediate_texture.get_render_target_view());

			p_context.m_render_thread.bind_render_pass_state(*l_step.m_ver_blur_render_pass_state);
			p_context.m_render_thread.bind_render_state(*l_step.m_ver_blur_render_state);
			p_context.m_render_thread.draw(0, 6);
			p_context.m_render_thread.unbind_render_state(*l_step.m_ver_blur_render_state);
			p_context.m_render_thread.unbind_render_pass_state(*l_step.m_ver_blur_render_pass_state);

			l_blur_textures.push_back(std::move(l_result_intermediate_texture));
		}

		// Apply
		const auto l_upscale_intermediate_texture = get_intermediate_texture(m_upscale_intermediate_texture_config);
		auto l_step = std::begin(m_blur_steps);
		auto l_blur_texture = std::begin(l_blur_textures);

		for (auto l_i = 0U; l_step != std::end(m_blur_steps); ++l_i, ++l_step, ++l_blur_texture)
		{
			m_upscale_texture_view_link.set_as_resource_view(l_blur_texture->get_resource_view());
			m_upscale_render_view_link.set_as_render_target_view(l_upscale_intermediate_texture.get_render_target_view());

			p_context.m_render_thread.bind_render_pass_state(*m_upscale_render_pass_state);
			p_context.m_render_thread.bind_render_state(*m_upscale_render_state);
			p_context.m_render_thread.draw(0, 6);
			p_context.m_render_thread.unbind_render_state(*m_upscale_render_state);
			p_context.m_render_thread.unbind_render_pass_state(*m_upscale_render_pass_state);

			m_apply_texture_view_link.set_as_resource_view(l_blur_texture->get_resource_view());

			const _bc_glow_params_struct l_glow_params
			{
				m_glow_threshold,
				m_glow_intensity,
				static_cast<bcFLOAT>(l_i + 1) / m_blur_steps.size()
			};
			p_context.m_render_thread.update_subresource(*m_glow_params_buffer, 0, &l_glow_params, 0, 0);

			p_context.m_render_thread.bind_render_pass_state(*m_glow_apply_render_pass_state);
			p_context.m_render_thread.bind_render_state(*m_glow_apply_render_state);
			p_context.m_render_thread.draw(0, 6);
			p_context.m_render_thread.unbind_render_state(*m_glow_apply_render_state);
			p_context.m_render_thread.unbind_render_pass_state(*m_glow_apply_render_pass_state);
		}

		p_context.m_render_thread.finish();
	}

	void bc_glow_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
		// Release reference to back buffer before reset
		m_render_target_resource_view.reset();
	}

	void bc_glow_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		const auto l_render_target_texture = get_shared_resource_throw<graphic::bc_texture2d>(m_render_target_texture_param);
		const auto l_render_target_view = get_shared_resource_throw<graphic::bc_render_target_view>(m_render_target_view_param);
		const auto l_full_viewport = graphic::bc_viewport::default_config(l_render_target_texture.get_width(), l_render_target_texture.get_height());
		
		m_back_buffer_width = l_render_target_texture.get_width();
		m_back_buffer_height = l_render_target_texture.get_height();
		m_parameters_changed = true;

		const auto l_render_target_resource_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_texture_view(l_render_target_texture.get_format())
			.as_tex2d_shader_view(0, 1)
			.on_render_target_texture2d();
		m_render_target_resource_view = p_context.m_device.create_resource_view(l_render_target_texture, l_render_target_resource_view_config);

		m_blur_steps.clear();

		for (const auto& [l_step_size, l_kernel_size] : { std::make_tuple(4, 15), std::make_tuple(8, 15) })
		{
			auto l_step_data = _create_blur_step_data(p_context, m_glow_texture_link, l_step_size, l_kernel_size);
			m_blur_steps.push_back(std::move(l_step_data));
		}

		m_glow_intermediate_texture_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_texture2d
			(
				m_back_buffer_width / m_blur_steps[0].m_size,
				m_back_buffer_height / m_blur_steps[0].m_size,
				false,
				1,
				graphic::bc_format::R8G8B8A8_UNORM,
				graphic::bc_resource_usage::gpu_rw,
				core::bc_enum::mask_or
				({
					graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::render_target
				})
			)
			.as_render_target_texture();
		m_upscale_intermediate_texture_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_texture2d
			(
				m_back_buffer_width / 2,
				m_back_buffer_height / 2,
				false,
				1,
				graphic::bc_format::R8G8B8A8_UNORM,
				graphic::bc_resource_usage::gpu_rw,
				core::bc_enum::mask_or
				({
					graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::render_target
				})
			)
			.as_render_target_texture();

		const auto l_upscale_viewport = graphic::bc_viewport::default_config(m_upscale_intermediate_texture_config.get_width(), m_upscale_intermediate_texture_config.get_height());
		const auto l_first_step_viewport = m_blur_steps[0].m_hor_blur_render_pass_state->get_viewport();

		// Glow extract states
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
				graphic::bc_format::R8G8B8A8_UNORM
			},
			graphic::bc_format::unknown,
			game::bc_multi_sample_type::c1_q1
		);
		m_glow_extract_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			*m_glow_extract_device_pipeline_state,
			l_first_step_viewport,
			{
				graphic::bc_render_target_view_parameter(m_glow_texture_link)
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
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::pixel, *m_glow_params_buffer)
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

		// Glow upscale states
		m_upscale_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"glow_extract_vs",
			nullptr,
			nullptr,
			nullptr,
			"gaussian_blur_upscale_ps",
			game::bc_vertex_type::pos_tex,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0xffffffff,
			{
				graphic::bc_format::R8G8B8A8_UNORM
			},
			graphic::bc_format::unknown,
			game::bc_multi_sample_type::c1_q1
		);
		m_upscale_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			*m_upscale_device_pipeline_state,
			l_upscale_viewport,
			{
				graphic::bc_render_target_view_parameter(m_upscale_render_view_link)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, *m_linear_sampler)
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, m_upscale_texture_view_link)
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer()
			}
		);
		m_upscale_render_state = p_context.m_render_system.create_render_state
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

		// Glow apply states
		m_glow_apply_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"glow_extract_vs",
			nullptr,
			nullptr,
			nullptr,
			"glow_apply_ps",
			game::bc_vertex_type::pos_tex,
			game::bc_blend_type::add_preserve_alpha,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0xffffffff,
			{
				l_render_target_texture.get_format()
			},
			graphic::bc_format::unknown,
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
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, m_apply_texture_view_link)
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::pixel, *m_glow_params_buffer)
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

	void bc_glow_pass::config_changed(const game::bc_render_pass_config_change_context& p_context)
	{
		core::bc_any l_threshold_value;
		core::bc_any l_intensity_value;

		p_context.m_global_config
			.read_config_key("render_glow_threshold", l_threshold_value)
			.read_config_key("render_glow_intensity", l_intensity_value);

		auto [l_threshold_read, l_threshold] = l_threshold_value.cast_to_double();
		auto [l_intensity_read, l_intensity] = l_intensity_value.cast_to_double();

		if (l_threshold_read && l_intensity_read)
		{
			m_glow_threshold = static_cast<bcFLOAT>(l_threshold);
			m_glow_intensity = static_cast<bcFLOAT>(l_intensity);
			m_parameters_changed = true;
		}
	}

	void bc_glow_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_blur_steps.clear();

		m_glow_extract_device_pipeline_state.reset();
		m_glow_extract_render_pass_state.reset();
		m_glow_extract_render_state.reset();

		m_glow_apply_device_pipeline_state.reset();
		m_glow_apply_render_pass_state.reset();
		m_glow_apply_render_state.reset();

		m_point_sampler.reset();
		m_linear_sampler.reset();
		m_render_target_resource_view.reset();
		m_render_target_resource_view.reset();
	}

	_bc_blur_step_data bc_glow_pass::_create_blur_step_data(const game::bc_render_pass_reset_context& p_context, 
		graphic::bc_shader_parameter_link p_glow_texture, 
		bcUINT32 p_step_size, 
		bcUINT32 p_kernel_size)
	{
		const auto l_render_target_texture = get_shared_resource_throw<graphic::bc_texture2d>(m_render_target_texture_param);
		const auto l_texture_width = l_render_target_texture.get_width() / p_step_size;
		const auto l_texture_height = l_render_target_texture.get_height() / p_step_size;
		const auto l_view_port = graphic::bc_viewport::default_config(l_texture_width, l_texture_height);
		const auto l_hor_blur_shader_name = (core::bc_string_stream_frame() << "gaussian_blur_" << p_kernel_size << "_hor_ps").str();
		const auto l_ver_blur_shader_name = (core::bc_string_stream_frame() << "gaussian_blur_" << p_kernel_size << "_ver_ps").str();

		graphic::bc_shader_parameter_link l_intermediate_texture_link;
		graphic::bc_shader_parameter_link l_result_texture_link;

		auto l_hor_blur_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"glow_extract_vs",
			nullptr,
			nullptr,
			nullptr,
			l_hor_blur_shader_name.c_str(),
			game::bc_vertex_type::pos_tex,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0xffffffff,
			{
				graphic::bc_format::R8G8B8A8_UNORM
			},
			graphic::bc_format::unknown,
			game::bc_multi_sample_type::c1_q1
		);
		auto l_hor_blur_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			*l_hor_blur_device_pipeline_state,
			l_view_port,
			{
				graphic::bc_render_target_view_parameter(l_intermediate_texture_link)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, *m_point_sampler)
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, std::move(p_glow_texture))
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::pixel, *m_blur_params_buffer)
			}
		);
		auto l_hor_blur_render_state = p_context.m_render_system.create_render_state
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

		auto l_ver_blur_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"glow_extract_vs",
			nullptr,
			nullptr,
			nullptr,
			l_ver_blur_shader_name.c_str(),
			game::bc_vertex_type::pos_tex,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0xffffffff,
			{
				graphic::bc_format::R8G8B8A8_UNORM
			},
			graphic::bc_format::unknown,
			game::bc_multi_sample_type::c1_q1
		);
		auto l_ver_blur_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			*l_ver_blur_device_pipeline_state,
			l_view_port,
			{
				graphic::bc_render_target_view_parameter(l_result_texture_link)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, *m_point_sampler)
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_intermediate_texture_link)
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::pixel, *m_blur_params_buffer)
			}
		);
	 	auto l_ver_blur_render_state = p_context.m_render_system.create_render_state
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

		_bc_blur_step_data l_data;
		l_data.m_size = p_step_size;
		l_data.m_intermediate_texture_link = std::move(l_intermediate_texture_link);
		l_data.m_result_texture_link = std::move(l_result_texture_link);
		l_data.m_hor_blur_device_pipeline_state = std::move(l_hor_blur_device_pipeline_state);
		l_data.m_hor_blur_render_pass_state = std::move(l_hor_blur_render_pass_state);
		l_data.m_hor_blur_render_state = std::move(l_hor_blur_render_state);
		l_data.m_ver_blur_device_pipeline_state = std::move(l_ver_blur_device_pipeline_state);
		l_data.m_ver_blur_render_pass_state = std::move(l_ver_blur_render_pass_state);
		l_data.m_ver_blur_render_state = std::move(l_ver_blur_render_state);

		return l_data;
	}

	void bc_glow_pass::_update_parameters(game::bc_default_render_thread& p_render_thread)
	{
		const _bc_glow_params_struct l_parameters
		{
			m_glow_threshold,
			m_glow_intensity
		};
		p_render_thread.update_subresource(*m_glow_params_buffer, 0, &l_parameters, 0, 0);
	}
}