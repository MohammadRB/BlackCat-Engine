// [08/30/2016 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecuter.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "Game/System/Render/bcRenderPass.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "BlackCat/bcExport.h"
#include "BlackCat/Loader/bcComputeShaderLoader.h"
#include "BlackCat/Loader/bcComputeShaderLoader.h"
#include "BlackCat/Loader/bcComputeShaderLoader.h"
#include "BlackCat/Loader/bcComputeShaderLoader.h"

namespace black_cat
{
	class BC_BLACKCAT_DLL_EXP bc_terrain_pass_dx11 : public game::bc_irender_pass
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
		static constexpr bcUINT16 s_shader_thread_group_size = 32;
		static constexpr bcUINT16 s_chund_size = 64;

		bool m_run_chunk_info_shader;
		game::bc_icamera::extend m_camera_extends;

		graphic::bc_device_command_list_ptr m_command_list;
		graphic::bc_device_pipeline_state_ptr m_pipeline_state;
		graphic::bc_buffer_ptr m_parameter_cbuffer;
		game::bc_render_pass_state_ptr m_render_pass_state;
		graphic::bc_device_compute_state_ptr m_device_compute_state;
	};
}
