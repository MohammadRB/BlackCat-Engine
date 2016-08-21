// [02/26/2016 MRB]

#pragma once

#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecuter.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"
#include "Game/bcExport.h"
#include "Game/System/Render/bcRenderPass.h"
#include "Game/System/Render/bcRenderPassState.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_back_buffer_output_pass : public bc_irender_pass
		{
		public:
			bc_back_buffer_output_pass() = default;

			~bc_back_buffer_output_pass() = default;

			bc_back_buffer_output_pass(bc_back_buffer_output_pass&&) = default;

			bc_back_buffer_output_pass& operator=(const bc_back_buffer_output_pass&) = default;

			void initialize_resources(bc_render_system& p_render_system, graphic::bc_device* p_device) override;

			void update(core_platform::bc_clock::update_param p_clock_update_param) override;

			void initialize_frame(bc_render_system& p_render_system, graphic::bc_device_pipeline* p_pipeline, graphic::bc_device_command_executer* p_executer) override;

			void execute(bc_render_system& p_render_system, graphic::bc_device_pipeline* p_pipeline, graphic::bc_device_command_executer* p_executer) override;

			void destroy(graphic::bc_device* p_device) override;

			void before_reset(bc_render_system& p_render_system, graphic::bc_device* p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters) override;

			void after_reset(bc_render_system& p_render_system, graphic::bc_device* p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters) override;

			core::bc_string get_name() override;

		protected:

		private:
			graphic::bc_device_pipeline_state_ptr m_pipeline_state;
			graphic::bc_device_command_list_ptr m_command_list;
			bc_render_pass_state_ptr m_render_pass_state;
		};
	}
}
