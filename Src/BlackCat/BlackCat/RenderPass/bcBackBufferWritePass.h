// [03/27/2019 MRB]

#pragma once

#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "Game/System/Render/Pass/bcRenderPassState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_BLACKCAT_DLL bc_back_buffer_write_pass : public game::bc_irender_pass
	{
		BC_RENDER_PASS(back_buffer_write)

	public:
		explicit bc_back_buffer_write_pass(game::bc_render_pass_resource_variable p_input_texture);

		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_param& p_param) override;

		void initialize_frame(const game::bc_render_pass_render_param& p_param) override;

		void execute(const game::bc_render_pass_render_param& p_param) override;

		void before_reset(const game::bc_render_pass_reset_param& p_param) override;

		void after_reset(const game::bc_render_pass_reset_param& p_param) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
		game::bc_render_pass_resource_variable m_input_texture;

		graphic::bc_buffer_ptr m_vertices_buffer;
		graphic::bc_buffer_ptr m_indices_buffer;

		graphic::bc_sampler_state_ptr m_sampler_state;
		graphic::bc_render_target_view_ptr m_back_buffer_view;
		graphic::bc_resource_view_ptr m_input_texture_view;

		graphic::bc_device_command_list_ptr m_command_list;
		graphic::bc_device_pipeline_state_ptr m_pipeline_state;
		game::bc_render_pass_state_ptr m_render_pass_state;
		game::bc_render_state_ptr m_render_state;
	};
}