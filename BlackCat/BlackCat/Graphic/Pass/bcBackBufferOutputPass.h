// [02/26/2016 MRB]

#pragma once

#include "Graphic/Rendering/bcRenderPass.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecuter.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"

namespace black_cat
{
	namespace graphic
	{
		class bc_back_buffer_output_pass : public bc_irender_pass, public bc_idevice_listener
		{
		public:
			bc_back_buffer_output_pass() = default;

			~bc_back_buffer_output_pass() = default;

			bc_back_buffer_output_pass(bc_back_buffer_output_pass&&) = default;

			bc_back_buffer_output_pass& operator=(const bc_back_buffer_output_pass&) = default;

			void initialize_resources(bc_device* p_device) override;

			void update(bcFLOAT32 p_time_elapsed) override;

			void initialize_frame(bc_device_command_executer* p_executer, bc_device_pipeline* p_pipeline) override;

			void execute(bc_device_command_executer* p_executer, bc_device_pipeline* p_pipeline) override;

			void destroy(bc_device* p_device) override;

			core::bc_string get_name() override;

		protected:
			void set_render_parameters(bc_device_pipeline* p_pipeline) override;

			void before_reset(bc_device* p_device, bc_device_parameters& p_old_parameters, bc_device_parameters& p_new_parameters) override;

			void after_reset(bc_device* p_device, bc_device_parameters& p_old_parameters, bc_device_parameters& p_new_parameters) override;

		private:
			bc_device_command_list_ptr m_command_list;
			bc_device_pipeline_state_ptr m_pipeline_state;
			bc_viewport m_viewport;
			bc_texture2d_ptr m_depth_buffer;
			bc_depth_stencil_view_ptr m_depth_stencil_view;
			bc_render_target_view_ptr m_bb_render_target_view;
		};
	}
}
