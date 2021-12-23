// [01/01/2021 MRB]

#pragma once

#include "Core/Messaging/Query/bcQueryResult.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/Query/bcMainCameraRenderStateQuery.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_gbuffer_skinned_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(gb_skn)

	public:
		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_context) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;

		void execute(const game::bc_render_pass_render_context& p_context) override;

		void before_reset(const game::bc_render_pass_reset_context& p_context) override;

		void after_reset(const game::bc_render_pass_reset_context& p_context) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
		graphic::bc_device_pipeline_state_ref m_pipeline_state;
		graphic::bc_sampler_state_ref m_sampler_state;
		game::bc_render_pass_state_ptr m_render_pass_state;

		core::bc_query_result<game::bc_main_camera_render_state_query> m_render_states_query;
		game::bc_render_state_buffer m_render_states;
	};
}