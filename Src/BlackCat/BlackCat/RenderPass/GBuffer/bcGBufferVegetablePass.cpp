// [09/06/2019 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Messaging/Query/bcQueryManager.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/System/Render/State/bcStateConfigs.h"
#include "Game/Object/Scene/Component/bcVegetableMeshComponent.h"
#include "BlackCat/RenderPass/GBuffer/bcGBufferVegetablePass.h"
#include "BlackCat/bcConstant.h"

namespace black_cat
{
	void bc_gbuffer_vegetable_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();

		after_reset(game::bc_render_pass_reset_context
			(
				p_render_system,
				l_device,
				graphic::bc_device_parameters
				(
					0,
					0,
					graphic::bc_format::unknown,
					graphic::bc_texture_ms_config(1, 0)
				),
				graphic::bc_device_parameters
				(
					l_device.get_back_buffer_width(),
					l_device.get_back_buffer_height(),
					l_device.get_back_buffer_format(),
					l_device.get_back_buffer_texture().get_sample_count()
				)
			));
	}

	void bc_gbuffer_vegetable_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_gbuffer_vegetable_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
		if(m_leaf_render_states_query.is_executed())
		{
			m_leaf_render_states = m_leaf_render_states_query.get().get_render_state_buffer();
		}
		if (m_trunk_render_states_query.is_executed())
		{
			m_trunk_render_states = m_trunk_render_states_query.get().get_render_state_buffer();
		}

		m_leaf_render_states_query = core::bc_get_service<core::bc_query_manager>()->queue_query
		(
			game::bc_main_camera_render_state_query
			(
				game::bc_actor_render_camera(p_context.m_update_camera),
				p_context.m_frame_renderer.create_buffer()
			).only<game::bc_vegetable_mesh_component>(true)
		);
		m_trunk_render_states_query = core::bc_get_service<core::bc_query_manager>()->queue_query
		(
			game::bc_main_camera_render_state_query
			(
				game::bc_actor_render_camera(p_context.m_update_camera),
				p_context.m_frame_renderer.create_buffer()
			).only<game::bc_vegetable_mesh_component>(false)
		);
	}

	void bc_gbuffer_vegetable_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
		p_context.m_render_thread.start();
		
		// Render vegetable leafs
		p_context.m_render_thread.bind_render_pass_state(*m_leaf_render_pass_state);
		
		p_context.m_frame_renderer.render_buffer(p_context.m_render_thread, m_leaf_render_states, p_context.m_render_camera);

		p_context.m_render_thread.unbind_render_pass_state(*m_leaf_render_pass_state);

		// Render vegetable trunks
		p_context.m_render_thread.bind_render_pass_state(*m_trunk_render_pass_state);
				
		p_context.m_frame_renderer.render_buffer(p_context.m_render_thread, m_trunk_render_states, p_context.m_render_camera);

		p_context.m_render_thread.unbind_render_pass_state(*m_trunk_render_pass_state);

		p_context.m_render_thread.finish();
	}

	void bc_gbuffer_vegetable_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_gbuffer_vegetable_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		if
		(
			p_context.m_old_parameters.m_width == p_context.m_new_parameters.m_width && 
			p_context.m_old_parameters.m_height == p_context.m_new_parameters.m_height
		)
		{
			return;
		}

		const auto* l_depth_stencil = get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_depth_stencil_texture);
		const auto* l_diffuse_map = get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_1);
		const auto* l_normal_map = get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_2);
		const auto* l_specular_map = get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_3);
		const auto* l_depth_stencil_view = get_shared_resource<graphic::bc_depth_stencil_view>(constant::g_rpass_depth_stencil_render_view);
		const auto* l_diffuse_map_view = get_shared_resource<graphic::bc_render_target_view>(constant::g_rpass_render_target_render_view_1);
		const auto* l_normal_map_view = get_shared_resource<graphic::bc_render_target_view>(constant::g_rpass_render_target_render_view_2);
		const auto* l_specular_map_view = get_shared_resource<graphic::bc_render_target_view>(constant::g_rpass_render_target_render_view_3);
		const auto l_viewport = graphic::bc_viewport::default_config(p_context.m_device.get_back_buffer_width(), p_context.m_device.get_back_buffer_height());
		const auto l_sampler_config = graphic::bc_graphic_resource_builder().as_resource().as_sampler_state
		(
			graphic::bc_filter::min_mag_mip_linear,
			graphic::bc_texture_address_mode::wrap,
			graphic::bc_texture_address_mode::wrap,
			graphic::bc_texture_address_mode::wrap
		).as_sampler_state();

		m_sampler_state = p_context.m_device.create_sampler_state(l_sampler_config);
		m_leaf_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"gbuffer_vegetable_leaf_vs",
			nullptr,
			nullptr,
			nullptr,
			"gbuffer_vegetable_leaf_ps",
			game::bc_vertex_type::pos_tex_nor_tan,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_less, game::bc_depth_stencil_type::stencil_write }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0x1,
			{
				l_diffuse_map->get_format(),
				l_normal_map->get_format(),
				l_specular_map->get_format()
			},
			l_depth_stencil->get_format(),
			game::bc_multi_sample_type::c1_q1
		);
		m_trunk_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"gbuffer_vegetable_trunk_vs",
			nullptr,
			nullptr,
			nullptr,
			"gbuffer_ps",
			game::bc_vertex_type::pos_tex_nor_tan,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_less, game::bc_depth_stencil_type::stencil_write }),
			game::bc_rasterizer_type::fill_solid_cull_back,
			0x1,
			{
				l_diffuse_map->get_format(),
				l_normal_map->get_format(),
				l_specular_map->get_format()
			},
			l_depth_stencil->get_format(),
			game::bc_multi_sample_type::c1_q1
		);
		m_leaf_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			m_leaf_pipeline_state.get(),
			l_viewport,
			{
				*l_diffuse_map_view, *l_normal_map_view, *l_specular_map_view
			},
			*l_depth_stencil_view,
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_sampler_state.get())
			},
			{},
			{},
			{
				p_context.m_render_system.get_global_cbuffer()
			}
		);
		m_trunk_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			m_trunk_pipeline_state.get(),
			l_viewport,
			{
				*l_diffuse_map_view, *l_normal_map_view, *l_specular_map_view
			},
			*l_depth_stencil_view,
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_sampler_state.get())
			},
			{},
			{},
			{
				p_context.m_render_system.get_global_cbuffer()
			}
		);
	}

	void bc_gbuffer_vegetable_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_trunk_render_pass_state.reset();
		m_leaf_render_pass_state.reset();
		m_trunk_pipeline_state.reset();
		m_leaf_pipeline_state.reset();
		m_sampler_state.reset();
	}
}