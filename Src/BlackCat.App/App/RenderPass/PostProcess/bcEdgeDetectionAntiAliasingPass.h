// [05/04/2022 MRB]

#pragma once

#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_edge_detection_anti_aliasing_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(edge_aa)
		
	public:
		bc_edge_detection_anti_aliasing_pass(game::bc_render_pass_variable_t p_input_texture_view,
			game::bc_render_pass_variable_t p_render_target_texture, 
			game::bc_render_pass_variable_t p_render_target_view);

		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_context) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;

		void execute(const game::bc_render_pass_render_context& p_context) override;

		void before_reset(const game::bc_render_pass_reset_context& p_context) override;

		void after_reset(const game::bc_render_pass_reset_context& p_context) override;

		void destroy(game::bc_render_system& p_render_system) override;

		void config_changed(const game::bc_render_pass_config_change_context& p_context) override;

	private:
		void _update_parameters(game::bc_default_render_thread& p_render_thread);
		
		game::bc_render_pass_variable_t m_input_texture_view;
		game::bc_render_pass_variable_t m_render_target_texture;
		game::bc_render_pass_variable_t m_render_target_view;

		bool m_parameters_changed = false;
		bcFLOAT m_min_depth_threshold;
		bcFLOAT m_max_depth_threshold;
		bcFLOAT m_normal_threshold;
		bcUINT32 m_sample_count;

		graphic::bc_buffer_ref m_parameters_cbuffer;
		graphic::bc_sampler_state_ref m_point_sampler;

		graphic::bc_device_pipeline_state_ref m_device_pipeline_state;
		game::bc_render_pass_state_ptr m_render_pass_state;
		game::bc_render_state_ptr m_render_state;
	};
}
