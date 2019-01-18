// [12/06/2018 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Game/System/Render/bcRenderSystem.h"
#include "Game/Object/Scene/bcScene.h"
#include "BlackCat/RenderPass/bcShapeDrawPass.h"

namespace black_cat
{
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

		after_reset(p_render_system, l_device, l_old_parameters, l_new_parameters);
	}

	void bc_shape_draw_pass::update(const game::bc_render_system_update_param& p_update_param)
	{
	}

	void bc_shape_draw_pass::initialize_frame(game::bc_render_system& p_render_system, game::bc_render_thread& p_thread, game::bc_scene& p_scene)
	{
		p_thread.start(m_command_list.get());
		p_thread.bind_render_pass_state(m_render_pass_state.get());
	}

	void bc_shape_draw_pass::execute(game::bc_render_system& p_render_system, game::bc_render_thread& p_thread, game::bc_scene& p_scene)
	{
		auto& l_shape_drawer = p_render_system.get_shape_drawer();

		p_scene.render_debug_shapes(l_shape_drawer);
		l_shape_drawer.render(p_render_system, p_thread);

		p_render_system.render_all_instances(p_thread);
		p_render_system.clear_render_instances();

		p_thread.unbind_render_pass_state(m_render_pass_state.get());
		p_thread.finish();

		m_command_list->reset();
	}

	void bc_shape_draw_pass::before_reset(game::bc_render_system& p_render_system, graphic::bc_device& p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters)
	{
		if
		(
			p_old_parameters.m_width != p_new_parameters.m_width ||
			p_old_parameters.m_height != p_new_parameters.m_height
		)
		{
			m_render_pass_state.reset();
		}
	}

	void bc_shape_draw_pass::after_reset(game::bc_render_system& p_render_system, graphic::bc_device& p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters)
	{
		if
		(
			p_old_parameters.m_width != p_new_parameters.m_width ||
			p_old_parameters.m_height != p_new_parameters.m_height
		)
		{
			graphic::bc_texture2d l_back_buffer_texture = p_device.get_back_buffer_texture();

			const auto l_depth_stencil_view = *get_shared_resource< graphic::bc_depth_stencil_view >(game::bc_render_pass_resource_variable::depth_stencil_view);
			const auto l_render_target_view = *get_shared_resource< graphic::bc_render_target_view >(game::bc_render_pass_resource_variable::render_target_view);
			const auto l_viewport = graphic::bc_viewport::default_config(l_back_buffer_texture.get_width(), l_back_buffer_texture.get_height());
			
			m_render_pass_state = p_render_system.create_render_pass_state
			(
				m_pipeline_state.get(),
				l_viewport,
				{ l_render_target_view },
				l_depth_stencil_view,
				{},
				{},
				{}
			);
		}
	}

	void bc_shape_draw_pass::destroy(graphic::bc_device& p_device)
	{
		m_render_pass_state.reset();
		m_pipeline_state.reset();
		m_command_list.reset();
	}
}