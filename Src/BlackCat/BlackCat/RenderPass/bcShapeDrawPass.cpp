// [12/06/2018 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Messaging/Query/bcQueryManager.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Query/bcMainCameraSceneSharedQuery.h"
#include "BlackCat/RenderPass/bcShapeDrawPass.h"
#include "BlackCat/bcConstant.h"

namespace black_cat
{
	bc_scene_debug_shape_query::bc_scene_debug_shape_query(game::bc_shape_drawer& p_shape_drawer, const game::bc_actor& p_selected_actor) noexcept
		: bc_query(message_name()),
		m_shape_drawer(&p_shape_drawer),
		m_selected_actor(p_selected_actor)
	{
	}

	void bc_scene_debug_shape_query::execute(const game::bc_scene_query_context& p_context) noexcept
	{
		const auto& l_actors_buffer = p_context.get_shared_query<game::bc_main_camera_scene_shared_query>().get_scene_buffer();
		
		if(m_selected_actor.is_valid())
		{
			const auto l_ite = l_actors_buffer.find(m_selected_actor);
			if (l_ite != std::end(l_actors_buffer))
			{
				l_ite->draw_debug(*m_shape_drawer);
			}
		}

		/*for(auto& l_actor : l_actors_buffer)
		{
			l_actor.draw_debug(*m_shape_drawer);
		}*/
		
		p_context.m_scene->draw_debug_shapes(*m_shape_drawer);
	}
	
	bc_shape_draw_pass::bc_shape_draw_pass(game::bc_render_pass_variable_t p_render_target_view)
		: m_render_target_view_variable(p_render_target_view)
	{
	}

	void bc_shape_draw_pass::set_selected_actor(const game::bc_actor& p_actor)
	{
		m_selected_actor = p_actor;
	}

	void bc_shape_draw_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		auto& l_device_swap_buffer = p_render_system.get_device_swap_buffer();

		m_pipeline_state = p_render_system.create_device_pipeline_state
		(
			"shape_draw_vs",
			nullptr,
			nullptr,
			nullptr,
			"shape_draw_ps",
			game::bc_vertex_type::pos,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_less, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			1,
			{ l_device_swap_buffer.get_back_buffer_texture().get_format()},
			game::bc_surface_format_type::D32_FLOAT,
			game::bc_multi_sample_type::c1_q1
		);
		
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

	void bc_shape_draw_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_shape_draw_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
		m_scene_debug_query = core::bc_get_service<core::bc_query_manager>()->queue_query
		(
			bc_scene_debug_shape_query(p_context.m_render_system.get_shape_drawer(), m_selected_actor)
		);
	}

	void bc_shape_draw_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
		auto& l_shape_drawer = p_context.m_render_system.get_shape_drawer();
		
		p_context.m_render_thread.start();
		p_context.m_render_thread.bind_render_pass_state(*m_render_pass_state.get());

		auto l_render_state_buffer = p_context.m_frame_renderer.create_buffer();
		l_shape_drawer.render(p_context.m_render_system, p_context.m_render_thread, l_render_state_buffer);
		p_context.m_frame_renderer.render_buffer(p_context.m_render_thread, l_render_state_buffer, p_context.m_render_camera);

		p_context.m_render_thread.unbind_render_pass_state(*m_render_pass_state.get());
		p_context.m_render_thread.finish();
	}

	void bc_shape_draw_pass::cleanup_frame(const game::bc_render_pass_render_context& p_context)
	{
	}

	void bc_shape_draw_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
		if
		(
			p_context.m_old_parameters.m_width != p_context.m_new_parameters.m_width ||
			p_context.m_old_parameters.m_height != p_context.m_new_parameters.m_height
		)
		{
			m_render_pass_state.reset();
		}
	}

	void bc_shape_draw_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		if
		(
			p_context.m_old_parameters.m_width != p_context.m_new_parameters.m_width ||
			p_context.m_old_parameters.m_height != p_context.m_new_parameters.m_height
		)
		{
			const auto l_back_buffer_texture = p_context.m_device_swap_buffer.get_back_buffer_texture();

			const auto l_depth_stencil_view = *get_shared_resource< graphic::bc_depth_stencil_view >(constant::g_rpass_depth_stencil_render_view);
			const auto l_render_target_view = *get_shared_resource< graphic::bc_render_target_view >(m_render_target_view_variable);
			const auto l_viewport = graphic::bc_viewport::default_config(l_back_buffer_texture.get_width(), l_back_buffer_texture.get_height());
			
			m_render_pass_state = p_context.m_render_system.create_render_pass_state
			(
				m_pipeline_state.get(),
				l_viewport,
				{ l_render_target_view },
				l_depth_stencil_view,
				{},
				{},
				{},
				{ p_context.m_render_system.get_global_cbuffer() }
			);
		}
	}

	void bc_shape_draw_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_render_pass_state.reset();
		m_pipeline_state.reset();
	}
}