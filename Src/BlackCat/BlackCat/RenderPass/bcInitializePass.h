// [08/31/2016 MRB]

#pragma once

#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_BLACKCAT_DLL bc_initialize_pass : public game::bc_irender_pass
	{
		BC_RENDER_PASS(initialize_pass)

	public:
		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_param& p_update_param) override;
		
		void initialize_frame(const game::bc_render_pass_render_param& p_param) override;

		void execute(const game::bc_render_pass_render_param& p_param) override;

		void cleanup_frame(const game::bc_render_pass_render_param& p_param) override;

		void before_reset(const game::bc_render_pass_reset_param& p_param) override;

		void after_reset(const game::bc_render_pass_reset_param& p_param) override;

		void destroy(game::bc_render_system& p_render_system) override;

	protected:

	private:
		//graphic::bc_device_command_list_ptr m_command_list;
		graphic::bc_format m_depth_stencil_format;
		graphic::bc_format m_render_target_format;
		graphic::bc_texture2d_ptr m_depth_buffer;
		graphic::bc_depth_stencil_view_ptr m_depth_stencil_view;
		graphic::bc_render_target_view_ptr m_render_target_view;
	};
}