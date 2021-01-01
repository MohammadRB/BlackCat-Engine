// [11/09/2020 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/bcConstant.h"
#include "Core/Container/bcArray.h"
#include "GraphicImp/Device/bcDeviceTextRenderer.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_text_draw_pass : public game::bc_irender_pass
	{
		BC_RENDER_PASS(txt_drw)

	public:
		explicit bc_text_draw_pass(constant::bc_render_pass_variable_t p_back_buffer_view_parameter);
		
		void initialize_resources(game::bc_render_system& p_render_system) override;
		
		void update(const game::bc_render_pass_update_context& p_param) override;
		
		void initialize_frame(const game::bc_render_pass_render_context& p_param) override;
		
		void execute(const game::bc_render_pass_render_context& p_param) override;
		
		void before_reset(const game::bc_render_pass_reset_context& p_param) override;
		
		void after_reset(const game::bc_render_pass_reset_context& p_param) override;
		
		void destroy(game::bc_render_system& p_render_system) override;

	private:
		core::bc_array<const bcCHAR*, 3> m_counter_values = { "fps", "update_time", "render_time" };
		
		constant::bc_render_pass_variable_t m_back_buffer_view_parameter;
		graphic::bc_render_target_view m_back_buffer_view;
		
		core::bc_unique_ptr<graphic::bc_device_text_renderer> m_text_renderer;
	};
}