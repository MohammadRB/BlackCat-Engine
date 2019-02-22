// [08/30/2016 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecutor.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_BLACKCAT_DLL bc_terrain_pass_dx11 : public game::bc_irender_pass
	{
		BC_RENDER_PASS(terrain_pass)

	public:
		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_system_update_param& p_update_param) override;

		void initialize_frame(game::bc_render_system& p_render_system, game::bc_render_thread& p_thread, game::bc_scene& p_scene) override;

		void execute(game::bc_render_system& p_render_system, game::bc_render_thread& p_thread, game::bc_scene& p_scene) override;

		void before_reset(game::bc_render_system& p_render_system, graphic::bc_device& p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters) override;

		void after_reset(game::bc_render_system& p_render_system, graphic::bc_device& p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters) override;

		void destroy(graphic::bc_device& p_device) override;

		void update_chunk_infos();

	protected:

	private:
		static constexpr bcUINT16 s_shader_thread_group_size = 32;
		static constexpr bcUINT16 s_chunk_size = 64;

		bool m_run_chunk_info_shader = false;
		game::bc_icamera::extend m_camera_extends;

		graphic::bc_device_command_list_ptr m_command_list;
		graphic::bc_device_pipeline_state_ptr m_pipeline_state;
		graphic::bc_device_compute_state_ptr m_device_compute_state;
		graphic::bc_buffer_ptr m_parameter_cbuffer;
		graphic::bc_sampler_state_ptr m_height_map_sampler;
		graphic::bc_sampler_state_ptr m_texture_sampler;
		game::bc_render_pass_state_ptr m_render_pass_state;
	};
}