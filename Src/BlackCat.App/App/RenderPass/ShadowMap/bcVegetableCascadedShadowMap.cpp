// [10/04/2019 MRB]

#include "BlackCat/BlackCatPCH.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcVegetableMeshComponent.h"
#include "BlackCat/RenderPass/ShadowMap/bcVegetableCascadedShadowMapPass.h"

namespace black_cat
{
	bc_vegetable_cascaded_shadow_map_pass::bc_vegetable_cascaded_shadow_map_pass(const bc_base_cascaded_shadow_map_pass& p_other_instance) 
		: bc_base_cascaded_shadow_map_pass(&p_other_instance)
	{
	}

	bc_vegetable_cascaded_shadow_map_pass::bc_vegetable_cascaded_shadow_map_pass(constant::bc_render_pass_variable_t p_output_depth_buffers, 
		bcSIZE p_shadow_map_size,
		const std::initializer_list< std::tuple< bcSIZE, bcUBYTE > >& p_cascade_sizes):
		bc_base_cascaded_shadow_map_pass(p_output_depth_buffers, p_shadow_map_size, p_cascade_sizes)
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
			nullptr,
			game::bc_vertex_type::pos_tex_nor_tan,
			game::bc_blend_type::opaque,
			game::bc_depth_stencil_type::depth_less_stencil_off,
			game::bc_rasterizer_type::fill_solid_cull_none,
			0x01,
			{},
			graphic::bc_format::D32_FLOAT,
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
			game::bc_depth_stencil_type::depth_less_stencil_off,
			game::bc_rasterizer_type::fill_solid_cull_none,
			0x01,
			{},
			graphic::bc_format::D32_FLOAT,
			game::bc_multi_sample_type::c1_q1
		);
	}

	void bc_vegetable_cascaded_shadow_map_pass::execute_pass(const bc_cascaded_shadow_map_pass_render_param& p_param)
	{
		const game::bc_camera_frustum l_camera_frustum(p_param.m_cascade_camera);
		auto l_scene_buffer = p_param.m_scene.get_actors<game::bc_vegetable_mesh_component>(l_camera_frustum);
		
		// Render vegetable leafs
		const auto& l_leaf_render_pass_state = *p_param.m_render_pass_states[p_param.m_cascade_index * 2];
		p_param.m_render_thread.bind_render_pass_state(l_leaf_render_pass_state);

		p_param.m_render_thread.clear_buffers(core::bc_vector4f(1));

		l_scene_buffer.render_actors<game::bc_vegetable_mesh_component>(p_param.m_render_system, true);

		p_param.m_render_system.render_all_instances(p_param.m_render_thread, p_param.m_clock, p_param.m_cascade_camera);
		p_param.m_render_system.clear_render_instances();

		p_param.m_render_thread.unbind_render_pass_state(l_leaf_render_pass_state);

		// Render vegetable trunks
		const auto& l_trunk_render_pass_state = *p_param.m_render_pass_states[p_param.m_cascade_index * 2 + 1];
		p_param.m_render_thread.bind_render_pass_state(l_trunk_render_pass_state);

		l_scene_buffer.render_actors<game::bc_vegetable_mesh_component>(p_param.m_render_system, false);

		p_param.m_render_system.render_all_instances(p_param.m_render_thread, p_param.m_clock, p_param.m_cascade_camera);
		p_param.m_render_system.clear_render_instances();

		p_param.m_render_thread.unbind_render_pass_state(l_trunk_render_pass_state);
	}

	void bc_vegetable_cascaded_shadow_map_pass::destroy_pass(game::bc_render_system& p_render_system)
	{
		m_leaf_pipeline_state.reset();
		m_trunk_pipeline_state.reset();
	}

	core::bc_vector<game::bc_render_pass_state_ptr> bc_vegetable_cascaded_shadow_map_pass::create_render_pass_states(game::bc_render_system& p_render_system, 
		graphic::bc_texture2d p_depth,
		graphic::bc_depth_stencil_view p_depth_view, 
		bcUINT32 p_cascade_index)
	{
		const auto l_leaf_render_state = p_render_system.create_render_pass_state
		(
			m_leaf_pipeline_state.get(),
			graphic::bc_viewport::default_config(p_depth.get_width(), p_depth.get_height()),
			{},
			p_depth_view,
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
			p_depth_view,
			{},
			{},
			{
				p_render_system.get_global_cbuffer(),
			}
		);

		return { l_leaf_render_state, l_trunk_render_state };
	}
}
