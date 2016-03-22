// [03/10/2016 MRB]

#include "BlackCat/BlackCatPCH.h"
#include "GraphicImp/Resource/bcResourceConfig.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecuter.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"
#include "Graphic/Rendering/bcRenderPass.h"
#include "BlackCat/Graphic/Pass/bcBackBufferOutputPass.h"

namespace black_cat
{
	namespace graphic
	{
		void bc_back_buffer_output_pass::initialize_resources(bc_device* p_device)
		{
			p_device->register_listender(this);

			bc_device_pipeline_state_config l_pipeline_state_config;
			bc_texture2d* l_back_buffer_texture = p_device->get_back_buffer_texture();
						
			l_pipeline_state_config.m_vertex_shader = nullptr;
			l_pipeline_state_config.m_hull_shader = nullptr;
			l_pipeline_state_config.m_domain_shader = nullptr;
			l_pipeline_state_config.m_geometry_shader = nullptr;
			l_pipeline_state_config.m_pixel_shader = nullptr;
			l_pipeline_state_config.m_blend_state_config = bc_blend_state_config::default_config();
			l_pipeline_state_config.m_sample_mask = 0x0;
			l_pipeline_state_config.m_depth_stencil_state_config = bc_depth_stencil_state_config::default_config();
			l_pipeline_state_config.m_rasterizer_state_config = bc_rasterizer_state_config::default_config();
			l_pipeline_state_config.m_input_layout_config;
			l_pipeline_state_config.m_num_render_target = 1;
			l_pipeline_state_config.m_render_target_format[0] = l_back_buffer_texture->get_format();
			l_pipeline_state_config.m_depth_stencil_format = bc_format::D32_FLOAT;
			l_pipeline_state_config.m_sample_config.m_count = 1;
			l_pipeline_state_config.m_sample_config.m_quality = 0;

			m_command_list = p_device->create_command_list();
			m_pipeline_state = p_device->create_pipeline_state(&l_pipeline_state_config);

			bc_device_parameters l_old_parameters
				(
					0,
					0,
					bc_format::UNKNOWN,
					bc_texture_ms_config(1, 0)
				);
			bc_device_parameters l_new_parameters
				(
					l_back_buffer_texture->get_width(),
					l_back_buffer_texture->get_height(),
					l_back_buffer_texture->get_format(),
					l_back_buffer_texture->get_sample_count()
				);

			after_reset(p_device, l_old_parameters, l_new_parameters);
		}

		void bc_back_buffer_output_pass::update(bcFLOAT32 p_time_elapsed)
		{
		}

		void bc_back_buffer_output_pass::initialize_frame(bc_device_command_executer* p_executer, bc_device_pipeline* p_pipeline)
		{
			bc_irender_pass::initialize_frame(p_executer, p_pipeline);

			p_pipeline->bind_pipeline_state(m_pipeline_state.get());
			p_pipeline->bind_rs_viewports(1, &m_viewport);
			p_pipeline->bind_om_render_targets(1, { m_bb_render_target_view.get() }, m_depth_stencil_view.get());
			p_pipeline->pipeline_apply_states(bc_pipeline_stage::output_merger_stage);
			p_pipeline->clear_buffers(bc_vector4f(0, 0, 0, 0), 1, 0);

			p_pipeline->pipeline_set_default_states(bc_pipeline_stage::output_merger_stage);
			p_pipeline->pipeline_apply_states(bc_pipeline_stage::output_merger_stage);

			p_pipeline->finish_command_list(m_command_list.get());
			p_executer->excecute_command_list(m_command_list.get());

			m_command_list->reset();
		}

		void bc_back_buffer_output_pass::execute(bc_device_command_executer* p_executer, bc_device_pipeline* p_pipeline)
		{

		}

		void bc_back_buffer_output_pass::destroy(bc_device* p_device)
		{
			p_device->unregister_listener(this);

			m_bb_render_target_view.reset(nullptr);
			m_depth_stencil_view.reset(nullptr);
			m_depth_buffer.reset(nullptr);
		}

		core::bc_string bc_back_buffer_output_pass::get_name()
		{
			return "back_buffer_output_pass";
		}

		void bc_back_buffer_output_pass::set_render_parameters(bc_device_pipeline* p_pipeline)
		{
		}

		void bc_back_buffer_output_pass::before_reset(bc_device* p_device, bc_device_parameters& p_old_parameters, bc_device_parameters& p_new_parameters)
		{
			if (p_old_parameters.m_width != p_new_parameters.m_width ||
				p_old_parameters.m_height != p_new_parameters.m_height)
			{
				m_bb_render_target_view.reset(nullptr);
				m_depth_stencil_view.reset(nullptr);
				m_depth_buffer.reset(nullptr);
			}
		}

		void bc_back_buffer_output_pass::after_reset(bc_device* p_device, bc_device_parameters& p_old_parameters, bc_device_parameters& p_new_parameters)
		{
			if (p_old_parameters.m_width != p_new_parameters.m_width ||
				p_old_parameters.m_height != p_new_parameters.m_height)
			{
				bc_graphic_resource_configure l_resource_configure;
				bc_texture2d* l_back_buffer_texture = p_device->get_back_buffer_texture();

				auto l_depth_buffer_config = l_resource_configure.as_resource().as_texture2d
					(
						l_back_buffer_texture->get_width(),
						l_back_buffer_texture->get_height(),
						false,
						1,
						m_pipeline_state->get_config().m_depth_stencil_format,
						bc_resource_usage::gpu_rw,
						bc_resource_view_type::depth_stencil
					)
					.as_depth_stencil_texture();
				auto l_depth_view_config = l_resource_configure.as_resource_view()
					.as_texture_view(bc_format::D32_FLOAT)
					.as_tex2d_depth_stencil_view(0);
				auto l_render_target_config = l_resource_configure.as_resource_view()
					.as_texture_view(m_pipeline_state->get_config().m_render_target_format[0])
					.as_tex2d_render_target_view(0);

				m_viewport = bc_viewport::default_config(l_back_buffer_texture->get_width(), l_back_buffer_texture->get_height());
				m_depth_buffer = p_device->create_texture2d(&l_depth_buffer_config, static_cast< bc_subresource_data* >(nullptr));
				m_depth_stencil_view = p_device->create_depth_stencil_view(m_depth_buffer.get(), &l_depth_view_config);
				m_bb_render_target_view = p_device->create_render_target_view(l_back_buffer_texture, &l_render_target_config);
			}
		}
	}
}
