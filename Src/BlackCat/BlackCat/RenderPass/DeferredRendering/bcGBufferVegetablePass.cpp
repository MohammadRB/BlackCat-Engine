// [09/06/2019 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/State/bcStateConfigs.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "Game/Object/Scene/Component/bcVegetableMeshComponent.h"
#include "BlackCat/RenderPass/DeferredRendering/bcGBufferVegetablePass.h"

namespace black_cat
{
	void bc_gbuffer_vegetable_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();

		m_command_list = l_device.create_command_list();
		m_leaf_pipeline_state = p_render_system.create_device_pipeline_state
		(
			"gbuffer_vegetable_leaf_vs",
			nullptr,
			nullptr,
			nullptr,
			"gbuffer_vegetable_leaf_ps",
			game::bc_vertex_type::pos_tex_nor_tan,
			game::bc_blend_type::opaque,
			game::bc_depth_stencil_type::depth_less_stencil_off,
			game::bc_rasterizer_type::fill_solid_cull_none,
			0x1,
			{
				get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_1)->get_format(),
				get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_2)->get_format()
			},
			get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_depth_stencil_texture)->get_format(),
			game::bc_multi_sample_type::c1_q1
		);
		m_trunk_pipeline_state = p_render_system.create_device_pipeline_state
		(
			"gbuffer_vegetable_trunk_vs",
			nullptr,
			nullptr,
			nullptr,
			"gbuffer_ps",
			game::bc_vertex_type::pos_tex_nor_tan,
			game::bc_blend_type::opaque,
			game::bc_depth_stencil_type::depth_less_stencil_off,
			game::bc_rasterizer_type::fill_solid_cull_back,
			0x1,
			{
				get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_1)->get_format(),
				get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_2)->get_format()
			},
			get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_depth_stencil_texture)->get_format(),
			game::bc_multi_sample_type::c1_q1
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

		after_reset(game::bc_render_pass_reset_param(p_render_system, l_device, l_old_parameters, l_new_parameters));
	}

	void bc_gbuffer_vegetable_pass::update(const game::bc_render_pass_update_param& p_param)
	{
	}

	void bc_gbuffer_vegetable_pass::initialize_frame(const game::bc_render_pass_render_param& p_param)
	{
	}

	void bc_gbuffer_vegetable_pass::execute(const game::bc_render_pass_render_param& p_param)
	{
		auto* l_actors = get_shared_resource<game::bc_scene_graph_buffer>(constant::g_rpass_actor_list);

		p_param.m_render_thread.start(m_command_list.get());
		
		// Render vegetable leafs
		p_param.m_render_thread.bind_render_pass_state(*m_leaf_render_pass_state.get());
		
		l_actors->render_actors<game::bc_vegetable_mesh_component>(p_param.m_render_system, true);
		p_param.m_render_system.render_all_instances(p_param.m_render_thread, p_param.m_clock, p_param.m_camera);
		p_param.m_render_system.clear_render_instances();

		p_param.m_render_thread.unbind_render_pass_state(*m_leaf_render_pass_state.get());

		// Render vegetable trunks
		p_param.m_render_thread.bind_render_pass_state(*m_trunk_render_pass_state.get());
				
		l_actors->render_actors<game::bc_vegetable_mesh_component>(p_param.m_render_system, false);
		p_param.m_render_system.render_all_instances(p_param.m_render_thread, p_param.m_clock, p_param.m_camera);
		p_param.m_render_system.clear_render_instances();

		p_param.m_render_thread.unbind_render_pass_state(*m_trunk_render_pass_state.get());

		p_param.m_render_thread.finish();

		m_command_list->finished();
	}

	void bc_gbuffer_vegetable_pass::before_reset(const game::bc_render_pass_reset_param& p_param)
	{
	}

	void bc_gbuffer_vegetable_pass::after_reset(const game::bc_render_pass_reset_param& p_param)
	{
		if
		(
			p_param.m_old_parameters.m_width == p_param.m_new_parameters.m_width && 
			p_param.m_old_parameters.m_height == p_param.m_new_parameters.m_height
		)
		{
			return;
		}

		const auto* l_depth_stencil_view = get_shared_resource<graphic::bc_depth_stencil_view>(constant::g_rpass_depth_stencil_view);
		const auto* l_diffuse_map_view = get_shared_resource<graphic::bc_render_target_view>(constant::g_rpass_render_target_view_1);
		const auto* l_normal_map_view = get_shared_resource<graphic::bc_render_target_view>(constant::g_rpass_render_target_view_2);
		const auto l_viewport = graphic::bc_viewport::default_config(p_param.m_device.get_back_buffer_width(), p_param.m_device.get_back_buffer_height());
		const auto l_sampler_config = graphic::bc_graphic_resource_builder().as_resource().as_sampler_state
		(
			graphic::bc_filter::min_mag_mip_linear,
			graphic::bc_texture_address_mode::wrap,
			graphic::bc_texture_address_mode::wrap,
			graphic::bc_texture_address_mode::wrap
		).as_sampler_state();

		m_sampler_state = p_param.m_device.create_sampler_state(l_sampler_config);
		m_leaf_render_pass_state = p_param.m_render_system.create_render_pass_state
		(
			m_leaf_pipeline_state.get(),
			l_viewport,
			{ *l_diffuse_map_view, *l_normal_map_view },
			*l_depth_stencil_view,
			{ graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_sampler_state.get()) },
			{},
			{
				p_param.m_render_system.get_global_cbuffer()
			}
		);
		m_trunk_render_pass_state = p_param.m_render_system.create_render_pass_state
		(
			m_trunk_pipeline_state.get(),
			l_viewport,
			{ *l_diffuse_map_view, *l_normal_map_view },
			*l_depth_stencil_view,
			{ graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_sampler_state.get()) },
			{},
			{
				p_param.m_render_system.get_global_cbuffer()
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
		m_command_list.reset();
	}
}