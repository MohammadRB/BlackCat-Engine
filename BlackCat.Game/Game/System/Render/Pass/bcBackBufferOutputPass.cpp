// [03/10/2016 MRB]


#include "Game/GamePCH.h"

#include "Core/Utility/bcEnumOperand.h"
#include "GraphicImp/Resource/bcResourceConfig.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecuter.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"
#include "Game/System/Render/Pass/bcBackBufferOutputPass.h"
#include "Game/System/Render/bcRenderSystem.h"

namespace black_cat
{
	namespace game
	{
		void bc_back_buffer_output_pass::initialize_resources(bc_render_system& p_render_system, graphic::bc_device* p_device)
		{
			graphic::bc_texture2d* l_back_buffer_texture = p_device->get_back_buffer_texture();
			
			m_command_list = p_device->create_command_list();
			m_pipeline_state = p_render_system.create_device_pipeline_state
				(
					"mesh_vs",
					nullptr,
					nullptr,
					nullptr,
					"mesh_ps",
					bc_vertex_type::pos_tex_nor_tan,
					bc_blend_type::opaque,
					bc_depth_stencil_type::depth_less_stencil_off,
					bc_rasterizer_type::fill_solid_cull_back,
					0x1,
					{ l_back_buffer_texture->get_format() },
					bc_surface_format_type::D32_FLOAT,
					bc_multi_sample_type::c1_q1
				);

			graphic::bc_device_parameters l_old_parameters
				(
					0,
					0,
					graphic::bc_format::UNKNOWN,
					graphic::bc_texture_ms_config(1, 0)
				);
			graphic::bc_device_parameters l_new_parameters
				(
					l_back_buffer_texture->get_width(),
					l_back_buffer_texture->get_height(),
					l_back_buffer_texture->get_format(),
					l_back_buffer_texture->get_sample_count()
				);

			after_reset(p_render_system, p_device, l_old_parameters, l_new_parameters);
		}

		void bc_back_buffer_output_pass::update(core_platform::bc_clock::update_param p_clock_update_param)
		{
		}

		void bc_back_buffer_output_pass::initialize_frame(bc_render_system& p_render_system, graphic::bc_device_pipeline* p_pipeline, graphic::bc_device_command_executer* p_executer)
		{
			p_render_system.bind_render_pass_state(p_pipeline, m_render_pass_state);

			p_pipeline->clear_buffers(graphic::bc_vector4f(0, 0, 255, 0), 1, 0);
		}

		void bc_back_buffer_output_pass::execute(bc_render_system& p_render_system, graphic::bc_device_pipeline* p_pipeline, graphic::bc_device_command_executer* p_executer)
		{
			p_render_system.get_scence_graph().render(p_render_system, false);

			p_render_system.unbind_render_pass_state(p_pipeline, m_render_pass_state);

			p_pipeline->finish_command_list(m_command_list.get());
			p_executer->excecute_command_list(m_command_list.get());

			m_command_list->reset();
		}

		void bc_back_buffer_output_pass::destroy(graphic::bc_device* p_device)
		{
			m_render_pass_state.reset(nullptr);
			m_command_list.reset(nullptr);
			m_pipeline_state.reset(nullptr);
		}

		core::bc_string bc_back_buffer_output_pass::get_name()
		{
			return "back_buffer_output_pass";
		}

		void bc_back_buffer_output_pass::before_reset(bc_render_system& p_render_system, graphic::bc_device* p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters)
		{
			if (p_old_parameters.m_width != p_new_parameters.m_width ||
				p_old_parameters.m_height != p_new_parameters.m_height)
			{
				m_render_pass_state.reset(nullptr);
			}
		}

		void bc_back_buffer_output_pass::after_reset(bc_render_system& p_render_system, graphic::bc_device* p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters)
		{
			if (p_old_parameters.m_width != p_new_parameters.m_width ||
				p_old_parameters.m_height != p_new_parameters.m_height)
			{
				graphic::bc_graphic_resource_configure l_resource_configure;
				graphic::bc_texture2d* l_back_buffer_texture = p_device->get_back_buffer_texture();

				auto l_depth_buffer_config = l_resource_configure.as_resource().as_texture2d
					(
						l_back_buffer_texture->get_width(),
						l_back_buffer_texture->get_height(),
						false,
						1,
						m_pipeline_state->get_config().m_depth_stencil_format,
						graphic::bc_resource_usage::gpu_rw,
						graphic::bc_resource_view_type::depth_stencil
					)
					.as_depth_stencil_texture();
				auto l_depth_view_config = l_resource_configure.as_resource_view()
					.as_texture_view(graphic::bc_format::D32_FLOAT)
					.as_tex2d_depth_stencil_view(0);
				auto l_render_target_config = l_resource_configure.as_resource_view()
					.as_texture_view(m_pipeline_state->get_config().m_render_target_format[0])
					.as_tex2d_render_target_view(0);
				auto l_linear_sampler_config = bc_graphic_state_configs::bc_sampler_config(bc_sampler_type::filter_linear_linear_linear_address_wrap_wrap_wrap);

				auto l_viewport = graphic::bc_viewport::default_config(l_back_buffer_texture->get_width(), l_back_buffer_texture->get_height());
				auto l_depth_buffer = p_device->create_texture2d(l_depth_buffer_config, static_cast< graphic::bc_subresource_data* >(nullptr));
				auto l_depth_stencil_view = p_device->create_depth_stencil_view(l_depth_buffer.get(), l_depth_view_config);
				auto l_bb_render_target_view = p_device->create_render_target_view(l_back_buffer_texture, l_render_target_config);
				auto l_linear_sampler = p_device->create_sampler_state(l_linear_sampler_config);

				m_render_pass_state = p_render_system.create_render_pass_state
					(
						m_pipeline_state,
						l_viewport,
						{ l_bb_render_target_view },
						l_depth_stencil_view,
						{ graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, l_linear_sampler) },
						{},
						{}
					);
			}
		}
	}
}
