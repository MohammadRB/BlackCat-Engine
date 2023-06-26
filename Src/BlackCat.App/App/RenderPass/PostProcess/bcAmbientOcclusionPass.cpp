// [03/03/2023 MRB]

#include "App/AppPCH.h"

#include "Core/Math/bcCoordinate.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "App/RenderPass/PostProcess/bcAmbientOcclusionPass.h"
#include "App/bcConstant.h"

namespace black_cat
{
	struct _bc_params_struct
	{
		BC_CBUFFER_ALIGN
		bcUINT32 m_num_rays;
		bcUINT32 m_steps_per_ray;
		bcFLOAT m_strength_per_ray;
		bcFLOAT m_radius;
		BC_CBUFFER_ALIGN
		bcFLOAT m_attenuation;
		bcFLOAT m_bias;
		bcFLOAT m_tan_fov;
		BC_CBUFFER_ALIGN
		core::bc_vector4f m_view_frustum_vectors[4];
		BC_CBUFFER_ALIGN
		bcFLOAT m_sin_cos[32][4];
	};

	bc_ambient_occlusion_pass::bc_ambient_occlusion_pass(game::bc_render_pass_variable_t p_render_target_texture, game::bc_render_pass_variable_t p_render_target_view) noexcept
		: m_render_target_texture(p_render_target_texture),
		  m_render_target_view(p_render_target_view),
		  m_update_parameters(true),
		  m_num_rays(4),
		  m_steps_per_ray(4),
		  m_strength(1),
		  m_radius(1.0f),
		  m_attenuation(2.0f),
		  m_bias(0.1f),
		  m_tangent_fov(0),
		  m_intermediate_texture_config()
	{
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
		m_params_buffer = l_device.create_buffer(l_params_buffer_config, nullptr);
		m_params_buffer->set_debug_name("ao_params_cbuffer");

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

	void bc_ambient_occlusion_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_ambient_occlusion_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
	}

	void bc_ambient_occlusion_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
		p_context.m_render_thread.start();

		const auto l_tan_fov = std::tanf(*p_context.m_render_camera.get_fov() / 2);
		if (m_update_parameters || l_tan_fov - m_tangent_fov > 0.001f)
		{
			m_tangent_fov = l_tan_fov;
			_update_parameters(p_context.m_render_thread, p_context.m_render_camera);
		}

		p_context.m_render_thread.bind_render_pass_state(*m_ao_render_pass_state);
		p_context.m_render_thread.bind_render_state(*m_ao_render_state);
		p_context.m_render_thread.draw(0, 6);
		p_context.m_render_thread.unbind_render_state(*m_ao_render_state);
		p_context.m_render_thread.unbind_render_pass_state(*m_ao_render_pass_state);

		p_context.m_render_thread.finish();
	}

	void bc_ambient_occlusion_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_ambient_occlusion_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		const auto& l_depth_texture = get_shared_resource_throw<graphic::bc_texture2d>(constant::g_rpass_depth_stencil_texture);
		const auto& l_depth_read_view = get_shared_resource_throw<graphic::bc_resource_view>(constant::g_rpass_depth_stencil_read_view);
		const auto& l_normal_texture = get_shared_resource_throw<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_2);
		const auto& l_normal_read_view = get_shared_resource_throw<graphic::bc_resource_view>(constant::g_rpass_render_target_read_view_2);
		const auto& l_render_target_texture = get_shared_resource_throw<graphic::bc_texture2d>(m_render_target_texture);
		const auto& l_render_target_view = get_shared_resource_throw<graphic::bc_render_target_view>(m_render_target_view);
		const auto l_viewport = graphic::bc_viewport::default_config(l_depth_texture.get_width(), l_depth_texture.get_height());

		m_intermediate_texture_config = graphic::bc_graphic_resource_builder()
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
				graphic::bc_format::R8G8B8A8_UNORM
			},
			graphic::bc_format::unknown,
			game::bc_multi_sample_type::c1_q1
		);
		m_ao_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			*m_ao_device_pipeline_state,
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(l_render_target_view)
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
				graphic::bc_constant_buffer_parameter(1, core::bc_enum::mask_or({graphic::bc_shader_type::vertex, graphic::bc_shader_type::pixel}), *m_params_buffer)
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
	}

	void bc_ambient_occlusion_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_point_sampler.reset();
		m_linear_sampler.reset();
		m_params_buffer.reset();

		m_ao_device_pipeline_state.reset();
		m_ao_render_pass_state.reset();
		m_ao_render_state.reset();
	}

	void bc_ambient_occlusion_pass::_update_parameters(game::bc_default_render_thread& p_render_thread, const game::bc_camera_instance& p_camera)
	{
		_bc_params_struct l_parameters{ m_num_rays, m_steps_per_ray, m_strength / m_num_rays, m_radius, m_attenuation, m_bias, m_tangent_fov, {}, {} };

		const auto l_camera_extends = p_camera.get_extends();
		const auto l_top_left_vector = core::bc_vector3f::normalize(l_camera_extends[5] - l_camera_extends[1]);
		const auto l_top_right_vector = core::bc_vector3f::normalize(l_camera_extends[6] - l_camera_extends[2]);
		const auto l_bottom_right_vector = core::bc_vector3f::normalize(l_camera_extends[7] - l_camera_extends[3]);
		const auto l_bottom_left_vector = core::bc_vector3f::normalize(l_camera_extends[4] - l_camera_extends[0]);

		l_parameters.m_view_frustum_vectors[0] = core::bc_vector4f(l_top_left_vector, 1);
		l_parameters.m_view_frustum_vectors[1] = core::bc_vector4f(l_top_right_vector, 1);
		l_parameters.m_view_frustum_vectors[2] = core::bc_vector4f(l_bottom_right_vector, 1);
		l_parameters.m_view_frustum_vectors[3] = core::bc_vector4f(l_bottom_left_vector, 1);

		for (auto i = 0U; i < m_num_rays; ++i)
		{
			const auto l_radians = (static_cast<bcFLOAT>(i) / m_num_rays) * core::g_pi * 2;
			l_parameters.m_sin_cos[i][0] = std::sinf(l_radians);
			l_parameters.m_sin_cos[i][1] = std::cosf(l_radians);
		}

		p_render_thread.update_subresource(*m_params_buffer, 0, &l_parameters, 0, 0);
	}
}