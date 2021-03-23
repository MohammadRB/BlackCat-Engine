// [03/27/2019 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_back_buffer_write_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(bb_wrt)

	public:
		explicit bc_back_buffer_write_pass(constant::bc_render_pass_variable_t p_input_texture);

		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_param) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_param) override;

		void execute(const game::bc_render_pass_render_context& p_param) override;

		void before_reset(const game::bc_render_pass_reset_context& p_param) override;

		void after_reset(const game::bc_render_pass_reset_context& p_param) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
		constant::bc_render_pass_variable_t m_input_texture;

		graphic::bc_buffer_ref m_vertices_buffer;
		graphic::bc_buffer_ref m_indices_buffer;

		graphic::bc_sampler_state_ref m_sampler_state;
		graphic::bc_render_target_view_ref m_back_buffer_view;
		graphic::bc_resource_view_ref m_input_texture_view;

		graphic::bc_device_pipeline_state_ref m_pipeline_state;
		game::bc_render_pass_state_ptr m_render_pass_state;
		game::bc_render_state_ptr m_render_state;
	};
}