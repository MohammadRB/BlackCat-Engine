// [02/26/2016 MRB]

#pragma once

#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecutor.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/Pass/bcRenderPassState.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_mesh_draw_pass : public game::bc_irender_pass
	{
		BC_RENDER_PASS(back_buffer_output_pass)

	public:
		bc_mesh_draw_pass() = default;

		~bc_mesh_draw_pass() = default;

		bc_mesh_draw_pass(bc_mesh_draw_pass&&) = default;

		bc_mesh_draw_pass& operator=(const bc_mesh_draw_pass&) = default;

		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_param& p_update_param) override;

		void initialize_frame(const game::bc_render_pass_render_param& p_param) override;

		void execute(const game::bc_render_pass_render_param& p_param) override;

		void destroy(game::bc_render_system& p_render_system) override;

		void before_reset(const game::bc_render_pass_reset_param& p_param) override;

		void after_reset(const game::bc_render_pass_reset_param& p_param) override;

	protected:

	private:
		graphic::bc_device_command_list_ptr m_command_list;
		graphic::bc_device_pipeline_state_ptr m_pipeline_state;
		graphic::bc_sampler_state_ptr m_sampler_state;
		game::bc_render_pass_state_ptr m_render_pass_state;
	};
}