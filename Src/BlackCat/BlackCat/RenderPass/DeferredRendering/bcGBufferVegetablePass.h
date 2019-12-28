// [09/06/2019 MRB]

#pragma once

#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "Game/System/Render/Pass/bcRenderPassState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_gbuffer_vegetable_pass : public game::bc_irender_pass
	{
		BC_RENDER_PASS(gbuffer_vegetable_pass)

	public:
		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_param& p_param) override;

		void initialize_frame(const game::bc_render_pass_render_param& p_param) override;

		void execute(const game::bc_render_pass_render_param& p_param) override;

		void before_reset(const game::bc_render_pass_reset_param& p_param) override;

		void after_reset(const game::bc_render_pass_reset_param& p_param) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
		graphic::bc_device_command_list_ptr m_command_list;
		graphic::bc_sampler_state_ptr m_sampler_state;
		graphic::bc_device_pipeline_state_ptr m_leaf_pipeline_state;
		graphic::bc_device_pipeline_state_ptr m_trunk_pipeline_state;
		game::bc_render_pass_state_ptr m_leaf_render_pass_state;
		game::bc_render_pass_state_ptr m_trunk_render_pass_state;
	};
}