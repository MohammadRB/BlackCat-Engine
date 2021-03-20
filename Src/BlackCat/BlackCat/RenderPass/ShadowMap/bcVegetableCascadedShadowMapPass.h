// [10/04/2019 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/Query/bcSceneGraphRenderStateQuery.h"
#include "BlackCat/RenderPass/ShadowMap/bcBaseCascadedShadowMapPass.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_vegetable_cascaded_shadow_map_pass final : public bc_base_cascaded_shadow_map_pass
	{
		BC_RENDER_PASS(csm_veg)
		
	public:
		explicit bc_vegetable_cascaded_shadow_map_pass(const bc_base_cascaded_shadow_map_pass& p_other_instance);

		bc_vegetable_cascaded_shadow_map_pass(constant::bc_render_pass_variable_t p_output_depth_buffers,
			bcFLOAT p_shadow_map_multiplier,
			const std::initializer_list< std::tuple< bcSIZE, bcUBYTE > >& p_cascade_sizes);

	private:
		void initialize_pass(game::bc_render_system& p_render_system) override;

		void initialize_frame_pass(const bc_cascaded_shadow_map_pass_init_frame_param& p_param) override;
		
		void execute_pass(const bc_cascaded_shadow_map_pass_render_param& p_param) override;
		
		void destroy_pass(game::bc_render_system& p_render_system) override;
		
		core::bc_vector<game::bc_render_pass_state_ptr> create_render_pass_states(game::bc_render_system& p_render_system, 
			graphic::bc_texture2d p_depth,
			const core::bc_vector<graphic::bc_depth_stencil_view_ptr>& p_depth_views) override;

		graphic::bc_device_pipeline_state_ptr m_leaf_pipeline_state;
		graphic::bc_device_pipeline_state_ptr m_trunk_pipeline_state;
		graphic::bc_sampler_state_ptr m_sampler_state;
		core::bc_vector<core::bc_query_result<game::bc_scene_graph_render_state_query>> m_leaf_scene_queries;
		core::bc_vector<core::bc_query_result<game::bc_scene_graph_render_state_query>> m_trunk_scene_queries;
		core::bc_vector<game::bc_render_state_buffer> m_leaf_scene_query_results;
		core::bc_vector<game::bc_render_state_buffer> m_trunk_scene_query_results;
	};
}