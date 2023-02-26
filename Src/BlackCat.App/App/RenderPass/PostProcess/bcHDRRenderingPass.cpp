// [02/10/2023 MRB]

#include "App/AppPCH.h"

#include "Core/Utility/bcParameterPack.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "App/RenderPass/PostProcess/bcHDRRenderingPass.h"
#include "App/bcConstant.h"

namespace black_cat
{
	struct _bc_hdr_rendering_cbuffer
	{
		BC_CBUFFER_ALIGN
		bcUINT32 m_luminance_mipmap_count;
	};

	bc_hdr_rendering_pass::bc_hdr_rendering_pass(game::bc_render_pass_variable_t p_hdr_texture,
		game::bc_render_pass_variable_t p_hdr_texture_view,
		game::bc_render_pass_variable_t p_hdr_texture_render_view,
		game::bc_render_pass_variable_t p_output_texture,
		game::bc_render_pass_variable_t p_output_texture_read_view,
		game::bc_render_pass_variable_t p_output_texture_render_view)
		: m_hdr_texture_param(p_hdr_texture),
		  m_hdr_texture_read_view_param(p_hdr_texture_view),
		  m_hdr_texture_render_view_param(p_hdr_texture_render_view),
		  m_output_texture_param(p_output_texture),
		  m_output_texture_read_view_param(p_output_texture_read_view),
		  m_output_texture_render_view_param(p_output_texture_render_view),
		  m_hdr_enabled(true),
		  m_luminance_texture_config(),
		  m_adaptive_luminance_texture_config()
	{
		auto& l_global_config = bc_get_global_config();
		l_global_config
			.add_if_not_exist_config_key(constant::g_cng_render_hdr_enabled, core::bc_any(true))
			.add_if_not_exist_config_key(constant::g_cng_render_hdr_reference_intensity, core::bc_any(10.f))
			.flush_changes();

		auto l_hdr_config_value = core::bc_any();
		l_global_config.read_config_key(constant::g_cng_render_hdr_enabled, l_hdr_config_value);

		if (l_hdr_config_value.has_value())
		{
			m_hdr_enabled = l_hdr_config_value.as_throw<bool>();
		}
	}

	void bc_hdr_rendering_pass::initialize_resources(game::bc_render_system& p_render_system)
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

	void bc_hdr_rendering_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_hdr_rendering_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
	}

	void bc_hdr_rendering_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
		if (!m_hdr_enabled)
		{
			return;
		}

		const auto l_luminance_texture = get_intermediate_texture(m_luminance_texture_config);
		auto l_adaptive_luminance_texture = get_intermediate_texture(m_adaptive_luminance_texture_config);

		p_context.m_render_thread.start();

		// Luminance extraction
		m_luminance_render_target_link.set_as_render_target_view(l_luminance_texture.get_render_target_view());

		p_context.m_render_thread.bind_render_pass_state(*m_luminance_render_pass_state);
		p_context.m_render_thread.bind_render_state(*m_luminance_render_state);
		p_context.m_render_thread.draw(0, 6);
		p_context.m_render_thread.unbind_render_state(*m_luminance_render_state);
		p_context.m_render_thread.unbind_render_pass_state(*m_luminance_render_pass_state);

		auto l_luminance_texture_view = l_luminance_texture.get_resource_view();
		p_context.m_render_thread.generate_mips(l_luminance_texture_view);

		// Adaptive luminance
		m_last_luminance_texture_link.set_as_resource_view(m_last_luminance_texture.get_resource_view());
		m_curr_luminance_texture_link.set_as_resource_view(l_luminance_texture.get_resource_view());
		m_adaptive_luminance_render_target_link.set_as_render_target_view(l_adaptive_luminance_texture.get_render_target_view());

		p_context.m_render_thread.bind_render_pass_state(*m_adaptive_luminance_render_pass_state);
		p_context.m_render_thread.bind_render_state(*m_adaptive_luminance_render_state);
		p_context.m_render_thread.draw(0, 6);
		p_context.m_render_thread.unbind_render_state(*m_adaptive_luminance_render_state);
		p_context.m_render_thread.unbind_render_pass_state(*m_adaptive_luminance_render_pass_state);

		// Tone mapping
		m_adaptive_luminance_texture_link.set_as_resource_view(l_adaptive_luminance_texture.get_resource_view());

		p_context.m_render_thread.bind_render_pass_state(*m_tone_mapping_render_pass_state);
		p_context.m_render_thread.bind_render_state(*m_tone_mapping_render_state);
		p_context.m_render_thread.draw(0, 6);
		p_context.m_render_thread.unbind_render_state(*m_tone_mapping_render_state);
		p_context.m_render_thread.unbind_render_pass_state(*m_tone_mapping_render_pass_state);

		p_context.m_render_thread.finish();

		m_last_luminance_texture = std::move(l_adaptive_luminance_texture);
	}

	void bc_hdr_rendering_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
		// Release intermediate texture because it will be removed by the intermediate texture manager
		m_last_luminance_texture = game::bc_intermediate_texture_guard();
	}

	void bc_hdr_rendering_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		const auto& l_hdr_texture = get_shared_resource_throw<graphic::bc_texture2d>(m_hdr_texture_param);
		const auto& l_hdr_texture_read_view = get_shared_resource_throw<graphic::bc_resource_view>(m_hdr_texture_read_view_param);
		const auto& l_hdr_texture_render_view = get_shared_resource_throw<graphic::bc_render_target_view>(m_hdr_texture_render_view_param);

		if (!m_hdr_enabled)
		{
			// Pass the input texture variables to the output slots so that they can be used in the rest of the pipeline
			share_resource(m_output_texture_param, l_hdr_texture);
			share_resource(m_output_texture_read_view_param, l_hdr_texture_read_view);
			share_resource(m_output_texture_render_view_param, l_hdr_texture_render_view);

			return;
		}

		const auto l_hdr_texture_width = l_hdr_texture.get_width();
		const auto l_hdr_texture_height = l_hdr_texture.get_height();
		const auto l_luminance_texture_width = static_cast<bcUINT32>(std::pow(2, std::floorf(std::log2f(static_cast<bcFLOAT>(l_hdr_texture_width) / 16))));
		const auto l_luminance_texture_height = static_cast<bcUINT32>(std::pow(2, std::floorf(std::log2f(static_cast<bcFLOAT>(l_hdr_texture_height) / 16))));
		const auto l_luminance_view_port = graphic::bc_viewport::default_config(l_luminance_texture_width, l_luminance_texture_height);
		const auto l_adaptive_luminance_view_port = graphic::bc_viewport::default_config(1, 1);
		const auto l_view_port = graphic::bc_viewport::default_config(l_hdr_texture_width, l_hdr_texture_height);

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

		m_point_sampler = p_context.m_device.create_sampler_state(l_point_sampler_config);

		const auto l_cbuffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer
			(
				1,
				sizeof(_bc_hdr_rendering_cbuffer),
				graphic::bc_resource_usage::gpu_r,
				graphic::bc_resource_view_type::shader
			)
			.as_constant_buffer();

		const auto l_luminance_texture_mipmap_count = static_cast<bcUINT32>(std::log2f(std::max(l_luminance_texture_width, l_luminance_texture_height)));
		const auto l_cbuffer_value = _bc_hdr_rendering_cbuffer{ l_luminance_texture_mipmap_count };
		const auto l_cbuffer_data = graphic::bc_subresource_data(&l_cbuffer_value, 0, 0);
		m_cbuffer = p_context.m_device.create_buffer(l_cbuffer_config, &l_cbuffer_data);

		auto l_resource_builder = graphic::bc_graphic_resource_builder();
		m_luminance_texture_config = l_resource_builder
			.as_resource()
			.as_texture2d
			(
				std::max(l_luminance_texture_width, 1U),
				std::max(l_luminance_texture_height, 1U),
				true,
				0,
				graphic::bc_format::R16_FLOAT,
				graphic::bc_resource_usage::gpu_rw,
				core::bc_enum::mask_or({ graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::render_target })
			)
			.as_render_target_texture();
		m_adaptive_luminance_texture_config = l_resource_builder
			.as_resource()
			.as_texture2d
			(
				1U,
				1U,
				true,
				0,
				graphic::bc_format::R16_FLOAT,
				graphic::bc_resource_usage::gpu_rw,
				core::bc_enum::mask_or({ graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::render_target })
			)
			.as_render_target_texture();

		auto l_output_texture_config = l_resource_builder
			.as_resource()
			.as_texture2d
			(
				p_context.m_new_parameters.m_width,
				p_context.m_new_parameters.m_height,
				false,
				1,
				graphic::bc_format::R8G8B8A8_UNORM,
				graphic::bc_resource_usage::gpu_rw,
				core::bc_enum::mask_or({ graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::render_target }))
			.as_normal_texture();
		auto l_output_texture_read_view_config = l_resource_builder
			.as_resource_view()
			.as_texture_view(l_output_texture_config.get_format())
			.as_tex2d_shader_view(0, -1)
			.on_texture2d();
		auto l_output_texture_render_view_config = l_resource_builder
			.as_resource_view()
			.as_texture_view(l_output_texture_config.get_format())
			.as_tex2d_render_target_view(0);

		m_output_texture = p_context.m_device.create_texture2d(l_output_texture_config, nullptr);
		m_output_texture_read_view = p_context.m_device.create_resource_view(m_output_texture.get(), l_output_texture_read_view_config);
		m_output_texture_render_view = p_context.m_device.create_render_target_view(m_output_texture.get(), l_output_texture_render_view_config);

		// Luminance states
		m_luminance_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"hdr_quad_vs",
			nullptr,
			nullptr,
			nullptr,
			"hdr_luminance_ps",
			game::bc_vertex_type::pos_tex,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0xffffffff,
			{
				m_luminance_texture_config.get_format()
			},
			graphic::bc_format::unknown,
			game::bc_multi_sample_type::c1_q1
		);
		m_luminance_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			m_luminance_device_pipeline_state.get(),
			l_luminance_view_port,
			{
				graphic::bc_render_target_view_parameter(m_luminance_render_target_link)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_point_sampler.get())
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_hdr_texture_read_view)
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
			}
		);
		m_luminance_render_state = p_context.m_render_system.create_render_state
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

		// Adaptive luminance states
		m_adaptive_luminance_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"hdr_quad_vs",
			nullptr,
			nullptr,
			nullptr,
			"hdr_adaptive_luminance_ps",
			game::bc_vertex_type::pos_tex,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0xffffffff,
			{
				m_adaptive_luminance_texture_config.get_format()
			},
			graphic::bc_format::unknown,
			game::bc_multi_sample_type::c1_q1
		);
		m_adaptive_luminance_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			m_adaptive_luminance_device_pipeline_state.get(),
			l_adaptive_luminance_view_port,
			{
				graphic::bc_render_target_view_parameter(m_adaptive_luminance_render_target_link)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_point_sampler.get())
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, graphic::bc_resource_view()),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::pixel, m_last_luminance_texture_link),
				graphic::bc_resource_view_parameter(2, graphic::bc_shader_type::pixel, m_curr_luminance_texture_link)
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::pixel, m_cbuffer.get())
			}
		);
		m_adaptive_luminance_render_state = p_context.m_render_system.create_render_state
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

		// Tone mapping states
		m_tone_mapping_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"hdr_quad_vs",
			nullptr,
			nullptr,
			nullptr,
			"hdr_tone_map_ps",
			game::bc_vertex_type::pos_tex,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0xffffffff,
			{
				l_output_texture_config.get_format()
			},
			graphic::bc_format::unknown,
			game::bc_multi_sample_type::c1_q1
		);
		m_tone_mapping_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			m_tone_mapping_device_pipeline_state.get(),
			l_view_port,
			{
				graphic::bc_render_target_view_parameter(m_output_texture_render_view.get())
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_point_sampler.get())
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_hdr_texture_read_view),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::pixel, graphic::bc_resource_view()),
				graphic::bc_resource_view_parameter(2, graphic::bc_shader_type::pixel, graphic::bc_resource_view()),
				graphic::bc_resource_view_parameter(3, graphic::bc_shader_type::pixel, m_adaptive_luminance_texture_link)
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::pixel, m_cbuffer.get())
			}
		);
		m_tone_mapping_render_state = p_context.m_render_system.create_render_state
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

		share_resource(m_output_texture_param, m_output_texture.get());
		share_resource(m_output_texture_read_view_param, m_output_texture_read_view.get());
		share_resource(m_output_texture_render_view_param, m_output_texture_render_view.get());
	}

	void bc_hdr_rendering_pass::config_changed(const game::bc_render_pass_config_change_context& p_context)
	{
		core::bc_any l_hdr_enabled_value;

		p_context.m_global_config.read_config_key(constant::g_cng_render_hdr_enabled, l_hdr_enabled_value);

		const auto l_hrd_enabled = bc_null_default(l_hdr_enabled_value.as<bool>(), m_hdr_enabled);

		if (!m_hdr_enabled && l_hrd_enabled)
		{
			request_device_reset(graphic::bc_device_parameters
			(
				p_context.m_device_swap_buffer.get_back_buffer_width(),
				p_context.m_device_swap_buffer.get_back_buffer_height(),
				p_context.m_device_swap_buffer.get_back_buffer_format(),
				p_context.m_device_swap_buffer.get_back_buffer_texture().get_sample_count()
			));
		}
		else if (m_hdr_enabled && !l_hrd_enabled)
		{
			destroy(p_context.m_render_system);
		}

		m_hdr_enabled = l_hrd_enabled;
	}

	void bc_hdr_rendering_pass::destroy(game::bc_render_system& p_render_system)
	{
		unshare_resource(m_output_texture_param);
		unshare_resource(m_output_texture_read_view_param);
		unshare_resource(m_output_texture_render_view_param);

		m_point_sampler.reset();
		m_cbuffer.reset();
		m_last_luminance_texture = game::bc_intermediate_texture_guard();

		m_output_texture.reset();
		m_output_texture_read_view.reset();
		m_output_texture_render_view.reset();

		m_luminance_device_pipeline_state.reset();
		m_luminance_render_pass_state.reset();
		m_luminance_render_state.reset();

		m_adaptive_luminance_device_pipeline_state.reset();
		m_adaptive_luminance_render_pass_state.reset();
		m_adaptive_luminance_render_state.reset();

		m_tone_mapping_device_pipeline_state.reset();
		m_tone_mapping_render_pass_state.reset();
		m_tone_mapping_render_state.reset();
	}
}
