// [12/06/2018 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Game/System/Render/bcRenderSystem.h"
#include "Game/Object/Scene/bcScene.h"
#include "BlackCat/RenderPass/bcShapeDrawPass.h"

namespace black_cat
{
	bc_shape_draw_pass::bc_shape_draw_pass(constant::bc_render_pass_variable_t p_render_target_view)
		: m_render_target_view_variable(p_render_target_view)
	{
	}

	void bc_shape_draw_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		graphic::bc_texture2d l_back_buffer_texture = l_device.get_back_buffer_texture();

		m_command_list = l_device.create_command_list();
		m_pipeline_state = p_render_system.create_device_pipeline_state
		(
			"shape_draw_vs",
			nullptr,
			nullptr,
			nullptr,
			"shape_draw_ps",
			game::bc_vertex_type::pos,
			game::bc_blend_type::opaque,
			game::bc_depth_stencil_type::depth_less_stencil_off,
			game::bc_rasterizer_type::fill_solid_cull_none,
			1,
			{ l_back_buffer_texture.get_format()},
			game::bc_surface_format_type::D32_FLOAT,
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
			l_back_buffer_texture.get_width(),
			l_back_buffer_texture.get_height(),
			l_back_buffer_texture.get_format(),
			l_back_buffer_texture.get_sample_count()
		);

		after_reset(game::bc_render_pass_reset_param(p_render_system, l_device, l_old_parameters, l_new_parameters));
	}

	void bc_shape_draw_pass::update(const game::bc_render_pass_update_param& p_update_param)
	{
	}

	void bc_shape_draw_pass::initialize_frame(const game::bc_render_pass_render_param& p_param)
	{
		p_param.m_render_thread.start(m_command_list.get());
		p_param.m_render_thread.bind_render_pass_state(m_render_pass_state.get());
	}

	void bc_shape_draw_pass::execute(const game::bc_render_pass_render_param& p_param)
	{
		auto& l_shape_drawer = p_param.m_render_system.get_shape_drawer();
		game::bc_scene_graph_buffer* l_actors = get_shared_resource<game::bc_scene_graph_buffer>(constant::g_rpass_actor_list);

		//p_param.m_scene.add_debug_shapes(l_shape_drawer, *l_actors);
		l_shape_drawer.render(p_param.m_render_system, p_param.m_render_thread);

		p_param.m_render_system.render_all_instances(p_param.m_render_thread, p_param.m_clock, p_param.m_camera);
		p_param.m_render_system.clear_render_instances();

		p_param.m_render_thread.unbind_render_pass_state(m_render_pass_state.get());
		p_param.m_render_thread.finish();

		m_command_list->finished();
	}

	void bc_shape_draw_pass::before_reset(const game::bc_render_pass_reset_param& p_param)
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

	void bc_shape_draw_pass::after_reset(const game::bc_render_pass_reset_param& p_param)
	{
		if
		(
			p_param.m_old_parameters.m_width != p_param.m_new_parameters.m_width ||
			p_param.m_old_parameters.m_height != p_param.m_new_parameters.m_height
		)
		{
			graphic::bc_texture2d l_back_buffer_texture = p_param.m_device.get_back_buffer_texture();

			const auto l_depth_stencil_view = *get_shared_resource< graphic::bc_depth_stencil_view >(constant::g_rpass_depth_stencil_view);
			const auto l_render_target_view = *get_shared_resource< graphic::bc_render_target_view >(m_render_target_view_variable);
			const auto l_viewport = graphic::bc_viewport::default_config(l_back_buffer_texture.get_width(), l_back_buffer_texture.get_height());
			
			m_render_pass_state = p_param.m_render_system.create_render_pass_state
			(
				m_pipeline_state.get(),
				l_viewport,
				{ l_render_target_view },
				l_depth_stencil_view,
				{},
				{},
				{ p_param.m_render_system.get_global_cbuffer() }
			);
		}
	}

	void bc_shape_draw_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_render_pass_state.reset();
		m_pipeline_state.reset();
		m_command_list.reset();
	}
}