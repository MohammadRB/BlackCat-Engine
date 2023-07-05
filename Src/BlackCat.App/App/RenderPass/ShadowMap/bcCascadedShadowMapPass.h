// [07/06/2019 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Container/bcDeque.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/Query/bcSceneGraphRenderStateQuery.h"
#include "App/RenderPass/ShadowMap/bcBaseCascadedShadowMapPass.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_cascaded_shadow_map_pass final : public bc_base_cascaded_shadow_map_pass
	{
		BC_RENDER_PASS(csm_sm)

	public:
		explicit bc_cascaded_shadow_map_pass(const bc_base_cascaded_shadow_map_pass& p_other_instance);

		bc_cascaded_shadow_map_pass(game::bc_render_pass_variable_t p_output_depth_buffers,
			bcFLOAT p_shadow_map_multiplier,
			std::initializer_list<bc_cascade_shadow_map_trait> p_cascade_sizes);

	private:
		void initialize_pass(game::bc_render_system& p_render_system) override;

		void initialize_frame_pass(const bc_cascaded_shadow_map_pass_init_frame_context& p_context) override;
		
		void execute_pass(const bc_cascaded_shadow_map_pass_render_context& p_context) override;

		void cleanup_frame_pass(const bc_cascaded_shadow_map_pass_cleanup_context& p_context) override;

		void destroy_pass(game::bc_render_system& p_render_system) override;

		core::bc_vector<game::bc_render_pass_state_ptr> create_render_pass_states(game::bc_render_system& p_render_system,
			graphic::bc_texture2d p_depth,
			const core::bc_vector<graphic::bc_depth_stencil_view_ref>& p_depth_views) override;

		graphic::bc_device_pipeline_state_ref m_device_pipeline;
		core::bc_deque<game::bc_scene_graph_render_state_query> m_scene_queries;
		core::bc_vector<core::bc_query_result<core::bci_query>> m_scene_query_results;
		core::bc_vector<game::bc_render_state_buffer> m_scene_render_states;
	};
}