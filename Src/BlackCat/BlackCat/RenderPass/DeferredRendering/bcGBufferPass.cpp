// [03/09/2019 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Game/System/Render/bcRenderSystem.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "BlackCat/RenderPass/DeferredRendering/bcGBufferPass.h"

namespace black_cat
{
	void bc_gbuffer_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();

		m_command_list = l_device.create_command_list();
		m_pipeline_state = p_render_system.create_device_pipeline_state
		(
			"gbuffer_vs",
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

	void bc_gbuffer_pass::update(const game::bc_render_pass_update_param& p_update_param)
	{
	}

	void bc_gbuffer_pass::initialize_frame(const game::bc_render_pass_render_param& p_param)
	{
		p_param.m_render_thread.start(m_command_list.get());
		p_param.m_render_thread.bind_render_pass_state(m_render_pass_state.get());
	}

	void bc_gbuffer_pass::execute(const game::bc_render_pass_render_param& p_param)
	{
		game::bc_scene_graph_buffer* l_actors = get_shared_resource<game::bc_scene_graph_buffer>(constant::g_rpass_actor_list);

		l_actors->render_actors<game::bc_mesh_component>(p_param.m_render_system);
		p_param.m_render_system.render_all_instances(p_param.m_render_thread, p_param.m_clock, p_param.m_camera);
		p_param.m_render_system.clear_render_instances();

		p_param.m_render_thread.unbind_render_pass_state(m_render_pass_state.get());
		p_param.m_render_thread.finish();

		m_command_list->finished();
	}

	void bc_gbuffer_pass::before_reset(const game::bc_render_pass_reset_param& p_param)
	{
		if
		(
			p_param.m_old_parameters.m_width != p_param.m_new_parameters.m_width ||
			p_param.m_old_parameters.m_height != p_param.m_new_parameters.m_height
		)
		{
			m_render_pass_state.reset();
		}
	}

	void bc_gbuffer_pass::after_reset(const game::bc_render_pass_reset_param& p_param)
	{
		if
		(
			p_param.m_old_parameters.m_width == p_param.m_new_parameters.m_width && p_param.m_old_parameters.m_height == p_param.m_new_parameters.m_height
		)
		{
			return;
		}

		const auto* l_depth_stencil_view = get_shared_resource<graphic::bc_depth_stencil_view>(constant::g_rpass_depth_stencil_view);
		const auto* l_diffuse_map_view = get_shared_resource<graphic::bc_render_target_view>(constant::g_rpass_render_target_view_1);
		const auto* l_normal_map_view = get_shared_resource<graphic::bc_render_target_view>(constant::g_rpass_render_target_view_2);
		const auto l_viewport = graphic::bc_viewport::default_config(p_param.m_device.get_back_buffer_width(), p_param.m_device.get_back_buffer_height());
		m_sampler_state = p_param.m_device.create_sampler_state(game::bc_graphic_state_configs::bc_sampler_config(game::bc_sampler_type::filter_linear_linear_linear_address_wrap_wrap_wrap));

		m_render_pass_state = p_param.m_render_system.create_render_pass_state
		(
			m_pipeline_state.get(),
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

	void bc_gbuffer_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_render_pass_state.reset();
		m_sampler_state.reset();
		m_pipeline_state.reset();
		m_command_list.reset();
	}
}