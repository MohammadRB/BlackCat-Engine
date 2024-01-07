// [06/02/2021 MRB]

#include "App/AppPCH.h"

#include "Core/Messaging/Query/bcQueryManager.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/System/Render/State/bcStateConfigs.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "App/RenderPass/ShadowMap/bcSkinnedCascadedShadowMapPass.h"

namespace black_cat
{
	bc_skinned_cascaded_shadow_map_pass::bc_skinned_cascaded_shadow_map_pass(const bc_base_cascaded_shadow_map_pass& p_other_instance)
		: bc_base_cascaded_shadow_map_pass(&p_other_instance)
	{
	}

	bc_skinned_cascaded_shadow_map_pass::bc_skinned_cascaded_shadow_map_pass(game::bc_render_pass_variable_t p_output_depth_buffers,
		bcFLOAT p_shadow_map_multiplier,
		std::initializer_list<bc_cascade_shadow_map_trait> p_cascade_sizes)
		: bc_base_cascaded_shadow_map_pass(p_output_depth_buffers, p_shadow_map_multiplier, p_cascade_sizes)
	{
	}

	void bc_skinned_cascaded_shadow_map_pass::initialize_pass(game::bc_render_system& p_render_system)
	{
		m_device_pipeline = p_render_system.create_device_pipeline_state
		(
			"skinned_mesh_shadow_map_vs",
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			game::bc_vertex_type::pos_tex_nor_tan_bon,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_less, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_back,
			0x01,
			{},
			graphic::bc_format::D16_UNORM,
			game::bc_multi_sample_type::c1_q1
		);
	}

	void bc_skinned_cascaded_shadow_map_pass::initialize_frame_pass(const bc_cascaded_shadow_map_pass_init_frame_context& p_context)
	{
		if (m_scene_query_results.size() < p_context.m_cascade_absolute_index + 1)
		{
			m_scene_queries.resize(p_context.m_cascade_absolute_index + 1);
			m_scene_query_results.resize(p_context.m_cascade_absolute_index + 1);
			m_scene_render_states.resize(p_context.m_cascade_absolute_index + 1);
		}

		if (m_scene_query_results[p_context.m_cascade_absolute_index].is_executed())
		{
			auto& l_scene_query_result = m_scene_query_results[p_context.m_cascade_absolute_index].get<game::bc_scene_graph_render_state_query>();
			m_scene_render_states[p_context.m_cascade_absolute_index] = l_scene_query_result.get_render_state_buffer();
		}

		if (p_context.m_render_next_frame)
		{
			m_scene_queries[p_context.m_cascade_absolute_index] = std::move
			(
				game::bc_scene_graph_render_state_query
				(
					game::bc_actor_render_camera(p_context.m_update_camera, p_context.m_update_cascade_camera),
					p_context.m_frame_renderer.create_buffer()
				)
				.with(game::bc_camera_frustum(p_context.m_update_cascade_camera))
				.only<game::bc_skinned_mesh_component>()
			);
			m_scene_query_results[p_context.m_cascade_absolute_index] = p_context.m_query_manager.queue_query_ref(m_scene_queries[p_context.m_cascade_absolute_index]);
		}
	}

	void bc_skinned_cascaded_shadow_map_pass::execute_pass(const bc_cascaded_shadow_map_pass_render_context& p_context)
	{
		const auto& l_render_buffer = m_scene_render_states[p_context.m_cascade_absolute_index];
		const auto& l_render_pass_state = *p_context.m_render_pass_states[p_context.m_cascade_index];

		// Update global cbuffer with cascade camera
		p_context.m_frame_renderer.update_global_cbuffer(p_context.m_child_render_thread, p_context.m_clock, p_context.m_render_cascade_camera);

		p_context.m_child_render_thread.bind_render_pass_state(l_render_pass_state);
		
		p_context.m_frame_renderer.render_skinned_buffer(p_context.m_child_render_thread, l_render_buffer, p_context.m_render_cascade_camera);

		p_context.m_child_render_thread.unbind_render_pass_state(l_render_pass_state);
	}

	void bc_skinned_cascaded_shadow_map_pass::cleanup_frame_pass(const bc_cascaded_shadow_map_pass_cleanup_context& p_context)
	{
		// Because shadow maps may not be updated in every frame we should clear render state buffers to not hold
		// reference to them for more than one frame
		m_scene_render_states[p_context.m_cascade_absolute_index] = game::bc_render_state_buffer();
	}

	void bc_skinned_cascaded_shadow_map_pass::destroy_pass(game::bc_render_system& p_render_system)
	{
		m_device_pipeline.reset();
		m_scene_query_results.clear();
		m_scene_render_states.clear();
	}

	core::bc_vector<game::bc_render_pass_state_ptr> bc_skinned_cascaded_shadow_map_pass::create_render_pass_states(game::bc_render_system& p_render_system,
		graphic::bc_texture2d p_depth,
		const core::bc_vector<graphic::bc_depth_stencil_view_ref>& p_depth_views)
	{
		core::bc_vector<game::bc_render_pass_state_ptr> l_result;
		l_result.reserve(p_depth_views.size());

		for (auto& l_depth_view : p_depth_views)
		{
			auto l_render_state = p_render_system.create_render_pass_state
			(
				m_device_pipeline.get(),
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

			l_result.push_back(std::move(l_render_state));
		}
		
		return l_result;
	}
}