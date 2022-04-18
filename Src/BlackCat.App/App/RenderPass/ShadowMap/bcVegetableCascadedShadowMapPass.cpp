// [10/04/2019 MRB]

#include "App/AppPCH.h"

#include "Core/Messaging/Query/bcQueryManager.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/State/bcStateConfigs.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcVegetableMeshComponent.h"
#include "App/RenderPass/ShadowMap/bcVegetableCascadedShadowMapPass.h"

namespace black_cat
{
	bc_vegetable_cascaded_shadow_map_pass::bc_vegetable_cascaded_shadow_map_pass(const bc_base_cascaded_shadow_map_pass& p_other_instance)
		: bc_base_cascaded_shadow_map_pass(&p_other_instance)
	{
	}

	bc_vegetable_cascaded_shadow_map_pass::bc_vegetable_cascaded_shadow_map_pass(game::bc_render_pass_variable_t p_output_depth_buffers,
		bcFLOAT p_shadow_map_multiplier,
		const std::initializer_list<bc_cascade_shadow_map_trait> p_cascade_sizes)
		: bc_base_cascaded_shadow_map_pass(p_output_depth_buffers, p_shadow_map_multiplier, p_cascade_sizes)
	{
	}

	void bc_vegetable_cascaded_shadow_map_pass::initialize_pass(game::bc_render_system& p_render_system)
	{
		m_leaf_pipeline_state = p_render_system.create_device_pipeline_state
		(
			"vegetable_leaf_shadow_map_vs",
			nullptr,
			nullptr,
			nullptr,
			"vegetable_leaf_shadow_map_ps",
			game::bc_vertex_type::pos_tex_nor_tan,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_less, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0x01,
			{},
			graphic::bc_format::D16_UNORM,
			game::bc_multi_sample_type::c1_q1
		);
		m_trunk_pipeline_state = p_render_system.create_device_pipeline_state
		(
			"vegetable_trunk_shadow_map_vs",
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			game::bc_vertex_type::pos_tex_nor_tan,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_less, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0x01,
			{},
			graphic::bc_format::D16_UNORM,
			game::bc_multi_sample_type::c1_q1
		);

		const auto l_sampler_config = graphic::bc_graphic_resource_builder().as_resource().as_sampler_state
		(
			graphic::bc_filter::min_mag_mip_linear,
			graphic::bc_texture_address_mode::wrap,
			graphic::bc_texture_address_mode::wrap,
			graphic::bc_texture_address_mode::wrap
		).as_sampler_state();
		m_sampler_state = p_render_system.get_device().create_sampler_state(l_sampler_config);
	}

	void bc_vegetable_cascaded_shadow_map_pass::initialize_frame_pass(const bc_cascaded_shadow_map_pass_init_frame_context& p_context)
	{
		if (m_leaf_query_results.size() < p_context.m_cascade_absolute_index + 1)
		{
			m_leaf_queries.resize(p_context.m_cascade_absolute_index + 1);
			m_leaf_query_results.resize(p_context.m_cascade_absolute_index + 1);
			m_leaf_render_states.resize(p_context.m_cascade_absolute_index + 1);
		}
		if (m_trunk_query_results.size() < p_context.m_cascade_absolute_index + 1)
		{
			m_trunk_queries.resize(p_context.m_cascade_absolute_index + 1);
			m_trunk_query_results.resize(p_context.m_cascade_absolute_index + 1);
			m_trunk_render_states.resize(p_context.m_cascade_absolute_index + 1);
		}

		if (m_leaf_query_results[p_context.m_cascade_absolute_index].is_executed())
		{
			m_leaf_render_states[p_context.m_cascade_absolute_index] = m_leaf_query_results[p_context.m_cascade_absolute_index].get<game::bc_scene_graph_render_state_query>().get_render_state_buffer();
		}
		if (m_trunk_query_results[p_context.m_cascade_absolute_index].is_executed())
		{
			m_trunk_render_states[p_context.m_cascade_absolute_index] = m_trunk_query_results[p_context.m_cascade_absolute_index].get<game::bc_scene_graph_render_state_query>().get_render_state_buffer();
		}

		if (p_context.m_render_next_frame)
		{
			m_leaf_queries[p_context.m_cascade_absolute_index] = std::move
			(
				game::bc_scene_graph_render_state_query
				(
					game::bc_actor_render_camera(p_context.m_update_camera, p_context.m_update_cascade_camera),
					p_context.m_frame_renderer.create_buffer()
				)
				.with(game::bc_camera_frustum(p_context.m_update_cascade_camera))
				.only<game::bc_vegetable_mesh_component>(true)
			);
			m_trunk_queries[p_context.m_cascade_absolute_index] = std::move
			(
				game::bc_scene_graph_render_state_query
				(
					game::bc_actor_render_camera(p_context.m_update_camera, p_context.m_update_cascade_camera),
					p_context.m_frame_renderer.create_buffer()
				)
				.with(game::bc_camera_frustum(p_context.m_update_cascade_camera))
				.only<game::bc_vegetable_mesh_component>(false)
			);

			m_leaf_query_results[p_context.m_cascade_absolute_index] = p_context.m_query_manager.queue_ext_query(m_leaf_queries[p_context.m_cascade_absolute_index]);
			m_trunk_query_results[p_context.m_cascade_absolute_index] = p_context.m_query_manager.queue_ext_query(m_trunk_queries[p_context.m_cascade_absolute_index]);
		}
	}

	void bc_vegetable_cascaded_shadow_map_pass::execute_pass(const bc_cascaded_shadow_map_pass_render_context& p_context)
	{
		const auto& l_leaf_render_buffer = m_leaf_render_states[p_context.m_cascade_absolute_index];
		const auto& l_trunk_render_buffer = m_trunk_render_states[p_context.m_cascade_absolute_index];
		
		// Update global cbuffer with cascade camera
		p_context.m_frame_renderer.update_global_cbuffer(p_context.m_child_render_thread, p_context.m_clock, p_context.m_render_cascade_camera);

		// Render vegetable leafs
		const auto& l_leaf_render_pass_state = *p_context.m_render_pass_states[p_context.m_cascade_index * 2];
		p_context.m_child_render_thread.bind_render_pass_state(l_leaf_render_pass_state);
		
		p_context.m_frame_renderer.render_buffer(p_context.m_child_render_thread, l_leaf_render_buffer, p_context.m_render_cascade_camera);

		p_context.m_child_render_thread.unbind_render_pass_state(l_leaf_render_pass_state);

		// Render vegetable trunks
		const auto& l_trunk_render_pass_state = *p_context.m_render_pass_states[p_context.m_cascade_index * 2 + 1];
		p_context.m_child_render_thread.bind_render_pass_state(l_trunk_render_pass_state);
		
		p_context.m_frame_renderer.render_buffer(p_context.m_child_render_thread, l_trunk_render_buffer, p_context.m_render_cascade_camera);

		p_context.m_child_render_thread.unbind_render_pass_state(l_trunk_render_pass_state);
	}

	void bc_vegetable_cascaded_shadow_map_pass::cleanup_frame_pass(const bc_cascaded_shadow_map_pass_cleanup_context& p_context)
	{
		// Because shadow maps may not be updated in every frame we should clear render state buffers to not hold
		// reference to them for more than one frame
		m_leaf_render_states[p_context.m_cascade_absolute_index] = game::bc_render_state_buffer();
		m_trunk_render_states[p_context.m_cascade_absolute_index] = game::bc_render_state_buffer();
	}

	void bc_vegetable_cascaded_shadow_map_pass::destroy_pass(game::bc_render_system& p_render_system)
	{
		m_leaf_pipeline_state.reset();
		m_trunk_pipeline_state.reset();

		m_leaf_queries.clear();
		m_trunk_queries.clear();
		m_leaf_query_results.clear();
		m_trunk_query_results.clear();
		m_leaf_render_states.clear();
		m_trunk_render_states.clear();
	}

	core::bc_vector<game::bc_render_pass_state_ptr> bc_vegetable_cascaded_shadow_map_pass::create_render_pass_states(game::bc_render_system& p_render_system,
		graphic::bc_texture2d p_depth,
		const core::bc_vector<graphic::bc_depth_stencil_view_ref>& p_depth_views)
	{
		core::bc_vector<game::bc_render_pass_state_ptr> l_result;
		l_result.reserve(p_depth_views.size() * 2);
		
		for (auto& l_depth_view : p_depth_views)
		{
			const auto l_leaf_render_state = p_render_system.create_render_pass_state
			(
				m_leaf_pipeline_state.get(),
				graphic::bc_viewport::default_config(p_depth.get_width(), p_depth.get_height()),
				{},
				l_depth_view.get(),
				{ graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_sampler_state.get()) },
				{},
				{},
				{
					p_render_system.get_global_cbuffer(),
				}
			);
			const auto l_trunk_render_state = p_render_system.create_render_pass_state
			(
				m_trunk_pipeline_state.get(),
				graphic::bc_viewport::default_config(p_depth.get_width(), p_depth.get_height()),
				{},
				l_depth_view.get(),
				{},
				{},
				{},
				{
					p_render_system.get_global_cbuffer(),
				}
			);

			l_result.push_back(l_leaf_render_state);
			l_result.push_back(l_trunk_render_state);
		}
				
		return l_result;
	}
}