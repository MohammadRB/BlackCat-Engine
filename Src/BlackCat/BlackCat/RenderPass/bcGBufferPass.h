// [03/09/2019 MRB]

#pragma once

#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/Pass/bcRenderPassState.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_BLACKCAT_DLL bc_gbuffer_pass : game::bc_irender_pass
	{
	public:
		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_param& p_update_param) override;

		void initialize_frame(const game::bc_render_pass_render_param& p_param) override;

		void execute(const game::bc_render_pass_render_param& p_param) override;

		void before_reset(const game::bc_render_pass_reset_param& p_param) override;

		void after_reset(const game::bc_render_pass_reset_param& p_param) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
		graphic::bc_device_command_list_ptr m_command_list;
		graphic::bc_device_pipeline_state_ptr m_pipeline_state;
		graphic::bc_sampler_state_ptr m_sampler_state;
		game::bc_render_pass_state_ptr m_render_pass_state;
	};
}