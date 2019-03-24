// [03/15/2019 MRB]

#pragma once

#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Device/bcDeviceComputeState.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/State/bcComputeState.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_BLACKCAT_DLL bc_gbuffer_final_pass : public game::bc_irender_pass
	{
		BC_RENDER_PASS(gbuffer_final_pass)

	public:
		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_param& p_param) override;

		void initialize_frame(const game::bc_render_pass_render_param& p_param) override;

		void execute(const game::bc_render_pass_render_param& p_param) override;

		void before_reset(const game::bc_render_pass_reset_param& p_param) override;

		void after_reset(const game::bc_render_pass_reset_param& p_param) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
		const bcSIZE m_shader_thread_group_size = 16;
		const bcSIZE m_num_direct_lights = 2;
		const bcSIZE m_num_point_lights = 100;
		const bcSIZE m_num_spot_lights = 100;

		graphic::bc_resource_view_ptr m_depth_stencil_view;
		graphic::bc_resource_view_ptr m_diffuse_map_view;
		graphic::bc_resource_view_ptr m_normal_map_view;

		graphic::bc_buffer_ptr m_direct_lights_buffer;
		graphic::bc_resource_view_ptr m_direct_lights_buffer_view;
		graphic::bc_buffer_ptr m_point_lights_buffer;
		graphic::bc_resource_view_ptr m_point_lights_buffer_view;
		graphic::bc_buffer_ptr m_spot_lights_buffer;
		graphic::bc_resource_view_ptr m_spot_lights_buffer_view;

		graphic::bc_texture2d_ptr m_output_texture;
		graphic::bc_resource_view_ptr m_output_texture_view;

		graphic::bc_device_command_list_ptr m_command_list;
		graphic::bc_device_compute_state_ptr m_device_compute_state;
		game::bc_compute_state_ptr m_compute_state;
	};
}