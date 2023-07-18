// [03/03/2023 MRB]

#include "App/AppPCH.h"

#include "Core/Math/bcCoordinate.h"
#include "Core/Utility/bcLogger.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "App/RenderPass/PostProcess/bcAmbientOcclusionPass.h"
#include "App/bcConstant.h"

namespace black_cat
{
	struct _bc_params_struct
	{
		constexpr static bcUINT s_sin_cos_array_size = 32;
		constexpr static bcUINT s_max_num_rays = s_sin_cos_array_size / 2;

		BC_CBUFFER_ALIGN
		bcUINT32 m_num_rays;
		bcUINT32 m_steps_per_ray;
		bcFLOAT m_strength;
		bcFLOAT m_radius;
		BC_CBUFFER_ALIGN
		bcFLOAT m_attenuation;
		bcFLOAT m_bias;
		bcFLOAT m_tan_fov;
		BC_CBUFFER_ALIGN
		bcFLOAT m_sin_cos[s_sin_cos_array_size][4];
	};

	struct _bc_frustum_params_struct
	{
		BC_CBUFFER_ALIGN
		core::bc_vector4f m_view_frustum_vectors[4];
	};

	struct _bc_blur_params_struct
	{
		BC_CBUFFER_ALIGN
		bcFLOAT m_texel_width;
		bcFLOAT m_texel_height;
		bcFLOAT m_blur_distance;
		bcFLOAT m_blur_angle;
	};

	bc_ambient_occlusion_pass::bc_ambient_occlusion_pass(game::bc_render_pass_variable_t p_input_texture_view,
		game::bc_render_pass_variable_t p_render_target_texture,
		game::bc_render_pass_variable_t p_render_target_view) noexcept
		: m_input_texture_view(p_input_texture_view),
		  m_render_target_texture(p_render_target_texture),
		  m_render_target_view(p_render_target_view),
		  m_update_parameters(true),
		  m_num_rays(4),
		  m_steps_per_ray(4),
		  m_strength(1.6f),
		  m_radius(1.5f),
		  m_attenuation(2.0f),
		  m_bias(0.1f),
		  m_tangent_fov(0),
		  m_blur_distance(0.5f),
		  m_blur_angle(0.8f),
		  m_intermediate_texture_config()
	{
		auto& l_global_config = bc_get_global_config();
		l_global_config
				.add_if_not_exist_config_key("render_ao_num_rays", core::bc_any(m_num_rays))
				.add_if_not_exist_config_key("render_ao_steps_per_ray", core::bc_any(m_steps_per_ray))
				.add_if_not_exist_config_key("render_ao_strength", core::bc_any(m_strength))
				.add_if_not_exist_config_key("render_ao_radius", core::bc_any(m_radius))
				.add_if_not_exist_config_key("render_ao_attenuation", core::bc_any(m_attenuation))
				.add_if_not_exist_config_key("render_ao_bias", core::bc_any(m_bias))
				.add_if_not_exist_config_key("render_ao_blur_distance", core::bc_any(m_blur_distance))
				.add_if_not_exist_config_key("render_ao_blur_angle", core::bc_any(m_blur_angle))
				.flush_changes();
	}

	void bc_ambient_occlusion_pass::initialize_resources(game::bc_render_system& p_render_system)
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

		const auto l_params_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(1, sizeof(_bc_params_struct), graphic::bc_resource_usage::gpu_rw)
			.as_constant_buffer();
		const auto l_frustum_params_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(1, sizeof(_bc_frustum_params_struct), graphic::bc_resource_usage::gpu_rw)
			.as_constant_buffer();
		const auto l_blur_params_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(1, sizeof(_bc_blur_params_struct), graphic::bc_resource_usage::gpu_rw)
			.as_constant_buffer();
		m_params_buffer = l_device.create_buffer(l_params_buffer_config, nullptr);
		m_params_buffer->set_debug_name("ao_params_cbuffer");
		m_frustum_params_buffer = l_device.create_buffer(l_frustum_params_buffer_config, nullptr);
		m_frustum_params_buffer->set_debug_name("ao_frustum_params_cbuffer");
		m_blur_params_buffer = l_device.create_buffer(l_blur_params_buffer_config, nullptr);
		m_blur_params_buffer->set_debug_name("ao_blur_params_cbuffer");

		after_reset(game::bc_render_pass_reset_context::create_default_instance(p_render_system, l_device, l_device_swap_buffer));
	}

	void bc_ambient_occlusion_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_ambient_occlusion_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
	}

	void bc_ambient_occlusion_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
		if (m_num_rays == 0 || m_steps_per_ray == 0)
		{
			unshare_resource(m_render_target_texture);
			unshare_resource(m_render_target_view);
			return;
		}

		share_resource(m_render_target_texture, *m_output_texture);
		share_resource(m_render_target_view, *m_output_texture_read_view);

		p_context.m_render_thread.start();

		const auto l_tan_fov = std::tanf(*p_context.m_render_camera.get_fov() / 2);
		const auto l_tan_diff_is_high = std::abs(l_tan_fov - m_tangent_fov) > 0.001f;
		if (m_update_parameters || l_tan_diff_is_high)
		{
			m_tangent_fov = l_tan_fov;
			_update_params(p_context.m_render_thread);
			m_update_parameters = false;
		}

		_update_frustum_params(p_context.m_render_thread, p_context.m_render_camera);
		
		const auto l_intermediate_texture = get_intermediate_texture(m_intermediate_texture_config);

		// AO stage
		p_context.m_render_thread.bind_render_pass_state(*m_ao_render_pass_state);
		p_context.m_render_thread.bind_render_state(*m_ao_render_state);
		p_context.m_render_thread.draw(0, 6);
		p_context.m_render_thread.unbind_render_state(*m_ao_render_state);
		p_context.m_render_thread.unbind_render_pass_state(*m_ao_render_pass_state);
		
		m_intermediate_texture_link.set_as_render_target_view(l_intermediate_texture.get_render_target_view());

		// Blur stage
		p_context.m_render_thread.bind_render_pass_state(*m_hor_blur_render_pass_state);
		p_context.m_render_thread.bind_render_state(*m_hor_blur_render_state);
		p_context.m_render_thread.draw(0, 6);
		p_context.m_render_thread.unbind_render_state(*m_hor_blur_render_state);
		p_context.m_render_thread.unbind_render_pass_state(*m_hor_blur_render_pass_state);
		
		m_intermediate_texture_link.set_as_resource_view(l_intermediate_texture.get_resource_view());

		p_context.m_render_thread.bind_render_pass_state(*m_ver_blur_render_pass_state);
		p_context.m_render_thread.bind_render_state(*m_ver_blur_render_state);
		p_context.m_render_thread.draw(0, 6);
		p_context.m_render_thread.unbind_render_state(*m_ver_blur_render_state);
		p_context.m_render_thread.unbind_render_pass_state(*m_ver_blur_render_pass_state);

		// Apply stage
		//m_intermediate_texture_link_1.set_as_resource_view(l_intermediate_texture_1.get_resource_view());

		//p_context.m_render_thread.bind_render_pass_state(*m_apply_render_pass_state);
		//p_context.m_render_thread.bind_render_state(*m_apply_render_state);
		//p_context.m_render_thread.draw(0, 6);
		//p_context.m_render_thread.unbind_render_state(*m_apply_render_state);
		//p_context.m_render_thread.unbind_render_pass_state(*m_apply_render_pass_state);

		p_context.m_render_thread.finish();
	}
	
	void bc_ambient_occlusion_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_ambient_occlusion_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		//const auto& l_input_texture_view = get_shared_resource_throw<graphic::bc_resource_view>(m_input_texture_view);
		const auto& l_depth_texture = get_shared_resource_throw<graphic::bc_texture2d>(constant::g_rpass_depth_stencil_texture);
		const auto& l_depth_read_view = get_shared_resource_throw<graphic::bc_resource_view>(constant::g_rpass_depth_stencil_read_view);
		const auto& l_normal_texture = get_shared_resource_throw<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_2);
		const auto& l_normal_read_view = get_shared_resource_throw<graphic::bc_resource_view>(constant::g_rpass_render_target_read_view_2);
		//const auto& l_render_target_texture = get_shared_resource_throw<graphic::bc_texture2d>(m_render_target_texture);
		//const auto& l_render_target_view = get_shared_resource_throw<graphic::bc_render_target_view>(m_render_target_view);
		const auto l_viewport = graphic::bc_viewport::default_config(l_depth_texture.get_width(), l_depth_texture.get_height());

		const auto l_output_texture_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_texture2d
			(
				l_viewport.m_width, 
				l_viewport.m_height, 
				false, 
				1, 
				graphic::bc_format::R8_UNORM, 
				graphic::bc_resource_usage::gpu_rw, 
				core::bc_enum::mask_or({ graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::render_target })
			)
			.as_render_target_texture();
		const auto l_output_texture_read_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_texture_view(l_output_texture_config.get_format())
			.as_tex2d_shader_view(0, 1)
			.on_render_target_texture2d();
		const auto l_output_texture_render_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_texture_view(l_output_texture_config.get_format())
			.as_tex2d_render_target_view(0);

		m_output_texture = p_context.m_device.create_texture2d(l_output_texture_config, nullptr);
		m_output_texture_read_view = p_context.m_device.create_resource_view(*m_output_texture, l_output_texture_read_view_config);
		m_output_texture_render_view = p_context.m_device.create_render_target_view(*m_output_texture, l_output_texture_render_view_config);

		m_intermediate_texture_config = l_output_texture_config;

		m_ao_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"ao_vs",
			nullptr,
			nullptr,
			nullptr,
			"ao_ps",
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
		m_ao_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			*m_ao_device_pipeline_state,
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(*m_output_texture_render_view)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, *m_point_sampler)
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_depth_read_view),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::pixel, l_normal_read_view)
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::pixel, *m_params_buffer),
				graphic::bc_constant_buffer_parameter
				(
					2,
					core::bc_enum::mask_or({graphic::bc_shader_type::vertex, graphic::bc_shader_type::pixel}),
					*m_frustum_params_buffer
				)
			}
		);
		m_ao_render_state = p_context.m_render_system.create_render_state
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

		m_hor_blur_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"ao_blur_vs",
			nullptr,
			nullptr,
			nullptr,
			"ao_hor_blur_ps",
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
		m_hor_blur_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			*m_hor_blur_device_pipeline_state,
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(m_intermediate_texture_link)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, *m_point_sampler),
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, *m_linear_sampler)
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_depth_read_view),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::pixel, l_normal_read_view),
				graphic::bc_resource_view_parameter(2, graphic::bc_shader_type::pixel, *m_output_texture_read_view)
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::pixel, *m_blur_params_buffer)
			}
		);
		m_hor_blur_render_state = p_context.m_render_system.create_render_state
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

		m_ver_blur_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"ao_blur_vs",
			nullptr,
			nullptr,
			nullptr,
			"ao_ver_blur_ps",
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
		m_ver_blur_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			*m_ver_blur_device_pipeline_state,
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(*m_output_texture_render_view)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, *m_point_sampler),
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, *m_linear_sampler)
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_depth_read_view),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::pixel, l_normal_read_view),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::pixel, m_intermediate_texture_link),
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::pixel, *m_blur_params_buffer)
			}
		);
		m_ver_blur_render_state = p_context.m_render_system.create_render_state
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

		/*m_apply_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"ao_blur_vs",
			nullptr,
			nullptr,
			nullptr,
			"ao_apply_ps",
			game::bc_vertex_type::pos_tex,
			game::bc_blend_type::blend_preserve_alpha,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0xffffffff,
			{
				l_render_target_texture.get_format()
			},
			graphic::bc_format::unknown,
			game::bc_multi_sample_type::c1_q1
		);
		m_apply_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			*m_apply_device_pipeline_state,
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(l_render_target_view)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, *m_point_sampler),
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, *m_linear_sampler)
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, graphic::bc_resource_view()),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::pixel, graphic::bc_resource_view()),
				graphic::bc_resource_view_parameter(2, graphic::bc_shader_type::pixel, m_intermediate_texture_link_1),
				graphic::bc_resource_view_parameter(3, graphic::bc_shader_type::pixel, graphic::bc_resource_view(l_input_texture_view)),
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer()
			}
		);
		m_apply_render_state = p_context.m_render_system.create_render_state
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
		);*/

		share_resource(m_render_target_texture, *m_output_texture);
		share_resource(m_render_target_view, *m_output_texture_read_view);
	}

	void bc_ambient_occlusion_pass::config_changed(const game::bc_render_pass_config_change_context& p_context)
	{
		core::bc_any l_rum_rays_value;
		core::bc_any l_steps_per_ray_value;
		core::bc_any l_strength_value;
		core::bc_any l_radius_value;
		core::bc_any l_attenuation_value;
		core::bc_any l_bias_value;
		core::bc_any l_blur_distance_value;
		core::bc_any l_blur_angle_value;

		p_context.m_global_config
			.read_config_key("render_ao_num_rays", l_rum_rays_value)
			.read_config_key("render_ao_steps_per_ray", l_steps_per_ray_value)
			.read_config_key("render_ao_strength", l_strength_value)
			.read_config_key("render_ao_radius", l_radius_value)
			.read_config_key("render_ao_attenuation", l_attenuation_value)
			.read_config_key("render_ao_bias", l_bias_value)
			.read_config_key("render_ao_blur_distance", l_blur_distance_value)
			.read_config_key("render_ao_blur_angle", l_blur_angle_value);

		auto [l_rum_rays_read, l_rum_rays] = l_rum_rays_value.cast_to_double();
		auto [l_steps_per_ray_read, l_steps_per_ray] = l_steps_per_ray_value.cast_to_double();
		auto [l_strength_read, l_strength] = l_strength_value.cast_to_double();
		auto [l_radius_read, l_radius] = l_radius_value.cast_to_double();
		auto [l_attenuation_read, l_attenuation] = l_attenuation_value.cast_to_double();
		auto [l_bias_read, l_bias] = l_bias_value.cast_to_double();
		auto [l_blur_distance_read, l_blur_distance] = l_blur_distance_value.cast_to_double();
		auto [l_blur_angle_value_read, l_blur_angle] = l_blur_angle_value.cast_to_double();

		if (!l_rum_rays_read ||
			!l_steps_per_ray_read ||
			!l_strength_read ||
			!l_radius_read ||
			!l_attenuation_read ||
			!l_bias_read ||
			!l_blur_distance_read ||
			!l_blur_angle_value_read)
		{
			core::bc_log(core::bc_log_type::error) << bcL("Couldn't read AO parameters from config.") << core::bc_lend;
			return;
		}

		m_num_rays = l_rum_rays;
		m_steps_per_ray = l_steps_per_ray;
		m_strength = l_strength;
		m_radius = l_radius;
		m_attenuation = l_attenuation;
		m_bias = l_bias;
		m_blur_distance = l_blur_distance;
		m_blur_angle = l_blur_angle;
		m_update_parameters = true;
	}

	void bc_ambient_occlusion_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_point_sampler.reset();
		m_linear_sampler.reset();
		m_params_buffer.reset();
		m_frustum_params_buffer.reset();
		m_blur_params_buffer.reset();

		m_ao_device_pipeline_state.reset();
		m_ao_render_pass_state.reset();
		m_ao_render_state.reset();

		m_hor_blur_device_pipeline_state.reset();
		m_hor_blur_render_pass_state.reset();
		m_hor_blur_render_state.reset();
		m_ver_blur_device_pipeline_state.reset();
		m_ver_blur_render_pass_state.reset();
		m_ver_blur_render_state.reset();

		m_apply_device_pipeline_state.reset();
		m_apply_render_pass_state.reset();
		m_apply_render_state.reset();

		unshare_resource(m_render_target_texture);
		unshare_resource(m_render_target_view);
	}

	void bc_ambient_occlusion_pass::_update_params(game::bc_default_render_thread& p_render_thread)
	{
		m_num_rays = std::min(m_num_rays, _bc_params_struct::s_max_num_rays);

		_bc_params_struct l_parameters{ m_num_rays, m_steps_per_ray, m_strength, m_radius, m_attenuation, m_bias, m_tangent_fov, {} };
		
		for (auto i = 0U; i < _bc_params_struct::s_max_num_rays; ++i)
		{
			const auto l_radians = (static_cast<bcFLOAT>(i) / _bc_params_struct::s_max_num_rays) * core::g_pi * 2;
			const auto l_first_repeat_index = i;
			const auto l_second_repeat_index = l_first_repeat_index + _bc_params_struct::s_max_num_rays;

			l_parameters.m_sin_cos[l_first_repeat_index][0] = std::sinf(l_radians);
			l_parameters.m_sin_cos[l_first_repeat_index][1] = std::cosf(l_radians);
			l_parameters.m_sin_cos[l_second_repeat_index][0] = std::sinf(l_radians);
			l_parameters.m_sin_cos[l_second_repeat_index][1] = std::cosf(l_radians);
		}

		p_render_thread.update_subresource(*m_params_buffer, 0, &l_parameters, 0, 0);

		const _bc_blur_params_struct l_blur_params
		{
			1.f / m_intermediate_texture_config.get_width(),
			1.f / m_intermediate_texture_config.get_height(),
			m_blur_distance,
			m_blur_angle
		};

		p_render_thread.update_subresource(*m_blur_params_buffer, 0, &l_blur_params, 0, 0);
	}

	void bc_ambient_occlusion_pass::_update_frustum_params(game::bc_default_render_thread& p_render_thread, const game::bc_camera_instance& p_camera)
	{
		_bc_frustum_params_struct l_params{};

		const auto l_camera_extends = p_camera.get_extends();
		const auto l_top_left_vector = core::bc_vector3f::normalize(l_camera_extends[5] - l_camera_extends[1]);
		const auto l_top_right_vector = core::bc_vector3f::normalize(l_camera_extends[6] - l_camera_extends[2]);
		const auto l_bottom_right_vector = core::bc_vector3f::normalize(l_camera_extends[7] - l_camera_extends[3]);
		const auto l_bottom_left_vector = core::bc_vector3f::normalize(l_camera_extends[4] - l_camera_extends[0]);

		l_params.m_view_frustum_vectors[0] = core::bc_vector4f(l_top_left_vector, 1);
		l_params.m_view_frustum_vectors[1] = core::bc_vector4f(l_top_right_vector, 1);
		l_params.m_view_frustum_vectors[2] = core::bc_vector4f(l_bottom_right_vector, 1);
		l_params.m_view_frustum_vectors[3] = core::bc_vector4f(l_bottom_left_vector, 1);

		p_render_thread.update_subresource(*m_frustum_params_buffer, 0, &l_params, 0, 0);
	}
}