// [12/17/2021 MRB]

#pragma once

#include "GraphicImp/Resource/View/bcResourceView.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_glow_pass : public game::bci_render_pass
	{
	public:
		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_context) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;

		void execute(const game::bc_render_pass_render_context& p_context) override;

		void before_reset(const game::bc_render_pass_reset_context& p_context) override;

		void after_reset(const game::bc_render_pass_reset_context& p_context) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
		graphic::bc_resource_view_ref m_back_buffer_view;

		graphic::bc_device_pipeline_state_ref m_glow_extract_device_pipeline_state;
		game::bc_render_pass_state_ptr m_glow_extract_render_pass_state;
		game::bc_render_state_ptr m_glow_extract_render_state;
		graphic::bc_device_compute_state_ref m_blur_device_compute_state;
		game::bc_compute_state_ptr m_blur_compute_state;
	};
}