// [05/04/2022 MRB]

#include "App/AppPCH.h"

#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "App/RenderPass/PostProcess/bcEdgeDetectionAntiAliasingPass.h"
#include "App/bcConstant.h"

namespace black_cat
{
	struct _bc_parameters_cbuffer
	{
		BC_CBUFFER_ALIGN
		bcFLOAT m_min_depth_threshold;
		bcFLOAT m_max_depth_threshold;
		bcFLOAT m_normal_threshold;
		bcUINT32 m_sample_count;
	};

	bc_edge_detection_anti_aliasing_pass::bc_edge_detection_anti_aliasing_pass(game::bc_render_pass_variable_t p_input_texture_view,
		game::bc_render_pass_variable_t p_render_target_texture,
		game::bc_render_pass_variable_t p_render_target_view)
		: m_input_texture_view(p_input_texture_view),
		m_render_target_texture(p_render_target_texture),
		m_render_target_view(p_render_target_view),
		m_min_depth_threshold(0),
		m_max_depth_threshold(0),
		m_normal_threshold(0),
		m_sample_count(0)
	{
		auto& l_global_config = bc_get_global_config();
		l_global_config
				.add_if_not_exist_config_key("render_edge_aa_min_depth_threshold", core::bc_any(.02f * l_global_config.get_scene_global_scale()))
				.add_if_not_exist_config_key("render_edge_aa_max_depth_threshold", core::bc_any(.16f * l_global_config.get_scene_global_scale()))
				.add_if_not_exist_config_key("render_edge_aa_normal_threshold", core::bc_any(.3f))
				.add_if_not_exist_config_key("render_edge_aa_sample_count", core::bc_any(4))
				.flush_changes();
	}

	void bc_edge_detection_anti_aliasing_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		auto& l_device_swap_buffer = p_render_system.get_device_swap_buffer();

		const auto l_parameters = _bc_parameters_cbuffer{ m_min_depth_threshold, m_max_depth_threshold, m_normal_threshold };
		const auto l_parameters_cbuffer_data = graphic::bc_subresource_data(&l_parameters, 0, 0);
		const auto l_parameters_cbuffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(1, sizeof(_bc_parameters_cbuffer), graphic::bc_resource_usage::gpu_rw, graphic::bc_resource_view_type::none)
			.as_constant_buffer();
		m_parameters_cbuffer = l_device.create_buffer(l_parameters_cbuffer_config, &l_parameters_cbuffer_data);

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
		m_point_sampler = l_device.create_sampler_state(l_point_sampler_config);

		after_reset(game::bc_render_pass_reset_context
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
		));
	}

	void bc_edge_detection_anti_aliasing_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_edge_detection_anti_aliasing_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
	}

	void bc_edge_detection_anti_aliasing_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
		if(m_sample_count == 0)
		{
			return;
		}

		p_context.m_render_thread.start();

		if(m_parameters_changed)
		{
			_update_parameters(p_context.m_render_thread);
		}

		p_context.m_render_thread.bind_render_pass_state(*m_render_pass_state);
		p_context.m_render_thread.bind_render_state(*m_render_state);
		p_context.m_render_thread.draw(0, 6);
		p_context.m_render_thread.unbind_render_state(*m_render_state);
		p_context.m_render_thread.unbind_render_pass_state(*m_render_pass_state);

		p_context.m_render_thread.finish();
	}

	void bc_edge_detection_anti_aliasing_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_edge_detection_anti_aliasing_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		const auto& l_depth_stencil_view = get_shared_resource_throw<graphic::bc_resource_view>(constant::g_rpass_depth_stencil_read_view);
		const auto& l_normal_map_view = get_shared_resource_throw<graphic::bc_resource_view>(constant::g_rpass_render_target_read_view_2);
		const auto& l_diffuse_map_view = get_shared_resource_throw<graphic::bc_resource_view>(m_input_texture_view);
		const auto& l_render_target_texture = get_shared_resource_throw<graphic::bc_texture2d>(m_render_target_texture);
		const auto& l_render_target_view = get_shared_resource_throw<graphic::bc_render_target_view>(m_render_target_view);
		const auto l_viewport = graphic::bc_viewport::default_config(l_render_target_texture.get_width(), l_render_target_texture.get_height());

		m_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"edge_aa_vs",
			nullptr,
			nullptr,
			nullptr,
			"edge_aa_ps",
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
		m_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			*m_device_pipeline_state,
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(l_render_target_view)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, *m_point_sampler)
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_depth_stencil_view),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::pixel, l_diffuse_map_view),
				graphic::bc_resource_view_parameter(2, graphic::bc_shader_type::pixel, l_normal_map_view),
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::pixel, *m_parameters_cbuffer)
			}
		);
		m_render_state = p_context.m_render_system.create_render_state
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

	void bc_edge_detection_anti_aliasing_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_parameters_cbuffer.reset();
		m_point_sampler.reset();

		m_device_pipeline_state.reset();
		m_render_pass_state.reset();
		m_render_state.reset();
	}

	void bc_edge_detection_anti_aliasing_pass::config_changed(const game::bc_render_pass_config_change_context& p_context)
	{
		bci_render_pass::config_changed(p_context);

		core::bc_any l_min_depth_threshold_value;
		core::bc_any l_max_depth_threshold_value;
		core::bc_any l_normal_threshold_value;
		core::bc_any l_sample_count_value;

		p_context.m_global_config
			.read_config_key("render_edge_aa_min_depth_threshold", l_min_depth_threshold_value)
			.read_config_key("render_edge_aa_max_depth_threshold", l_max_depth_threshold_value)
			.read_config_key("render_edge_aa_normal_threshold", l_normal_threshold_value)
			.read_config_key("render_edge_aa_sample_count", l_sample_count_value);

		auto [l_min_depth_read, l_min_depth] = l_min_depth_threshold_value.cast_to_double();
		auto [l_max_depth_read, l_max_depth] = l_max_depth_threshold_value.cast_to_double();
		auto [l_normal_threshold_read, l_normal_threshold] = l_normal_threshold_value.cast_to_double();
		auto [l_sample_count_read, l_sample_count] = l_sample_count_value.cast_to_int();

		if (l_min_depth_read && l_max_depth_read && l_normal_threshold_read && l_sample_count_read)
		{
			m_min_depth_threshold = static_cast<bcFLOAT>(l_min_depth);
			m_max_depth_threshold = static_cast<bcFLOAT>(l_max_depth);
			m_normal_threshold = static_cast<bcFLOAT>(l_normal_threshold);
			m_sample_count = static_cast<bcUINT32>(l_sample_count);

			if(m_sample_count <= 0)
			{
				m_sample_count = 0;
			}
			else if(m_sample_count <= 4)
			{
				m_sample_count = 4;
			}
			else
			{
				m_sample_count = 8;
			}

			m_parameters_changed = true;
		}
	}
	
	void bc_edge_detection_anti_aliasing_pass::_update_parameters(game::bc_default_render_thread& p_render_thread)
	{
		const auto l_parameters = _bc_parameters_cbuffer
		{
			m_min_depth_threshold, m_max_depth_threshold, m_normal_threshold, m_sample_count
		};
		p_render_thread.update_subresource(*m_parameters_cbuffer, 0, &l_parameters, 0, 0);
	}
}
