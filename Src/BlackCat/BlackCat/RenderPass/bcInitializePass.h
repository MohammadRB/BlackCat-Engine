// [08/31/2016 MRB]

#pragma once

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

		void update(const game::bc_render_system_update_param& p_update_param) override;
		
		void initialize_frame(game::bc_render_system& p_render_system, game::bc_render_thread& p_thread, game::bc_scene& p_scene) override;

		void execute(game::bc_render_system& p_render_system, game::bc_render_thread& p_thread, game::bc_scene& p_scene) override;

		void cleanup_frame(game::bc_render_system& p_render_system, game::bc_render_thread& p_thread, game::bc_scene& p_scene) override;

		void before_reset(game::bc_render_system& p_render_system, graphic::bc_device& p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters) override;

		void after_reset(game::bc_render_system& p_render_system, graphic::bc_device& p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters) override;

		void destroy(graphic::bc_device& p_device) override;

	protected:

	private:
		graphic::bc_format m_depth_stencil_format;
		graphic::bc_format m_render_target_format;
		graphic::bc_texture2d_ptr m_depth_buffer;
		graphic::bc_depth_stencil_view_ptr m_depth_stencil_view;
		graphic::bc_render_target_view_ptr m_render_target_view;
		core_platform::bc_clock::update_param m_last_clock{0, 0};
		const game::bc_icamera* m_last_camera = nullptr;
	};
}