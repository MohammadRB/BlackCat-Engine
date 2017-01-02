// [08/31/2016 MRB]

#pragma once

#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "Game/System/Render/bcRenderPass.h"
#include "BlackCat/bcExport.h"
#include "BlackCat/Loader/bcComputeShaderLoader.h"
#include "BlackCat/Loader/bcComputeShaderLoader.h"
#include "BlackCat/Loader/bcComputeShaderLoader.h"
#include "BlackCat/Loader/bcComputeShaderLoader.h"

namespace black_cat
{
	class BC_BLACKCAT_DLL_EXP bc_initialize_pass : public game::bc_irender_pass
	{
	public:
		void initialize_resources(game::bc_render_system& p_render_system, graphic::bc_device& p_device) override;

		void update(const game::bc_render_system_update_param& p_update_param) override;
		
		void initialize_frame(game::bc_render_system& p_render_system, game::bc_render_thread& p_thread) override;

		void execute(game::bc_render_system& p_render_system, game::bc_render_thread& p_thread) override;

		void before_reset(game::bc_render_system& p_render_system, graphic::bc_device& p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters) override;

		void after_reset(game::bc_render_system& p_render_system, graphic::bc_device& p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters) override;

		void destroy(graphic::bc_device& p_device) override;

		core::bc_string get_name() override;

	protected:

	private:
		graphic::bc_format m_depth_stencil_format;
		graphic::bc_format m_render_target_format;
		graphic::bc_texture2d_ptr m_depth_buffer;
		graphic::bc_depth_stencil_view_ptr m_depth_stencil_view;
		graphic::bc_render_target_view_ptr m_render_target_view;
	};
}
