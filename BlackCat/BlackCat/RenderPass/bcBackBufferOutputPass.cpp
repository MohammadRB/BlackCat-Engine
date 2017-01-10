// [03/10/2016 MRB]

#include "BlackCat/BlackCatPCH.h"
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
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderPassResourceShare.h"
#include "BlackCat/RenderPass/bcBackBufferOutputPass.h"
#include "BlackCat/Loader/bcComputeShaderLoader.h"
#include "BlackCat/Loader/bcComputeShaderLoader.h"
#include "BlackCat/Loader/bcComputeShaderLoader.h"
#include "BlackCat/Loader/bcComputeShaderLoader.h"

namespace black_cat
{
	void bc_back_buffer_output_pass::initialize_resources(game::bc_render_system& p_render_system, graphic::bc_device& p_device)
	{
		graphic::bc_texture2d l_back_buffer_texture = p_device.get_back_buffer_texture();

		m_command_list = p_device.create_command_list();
		m_pipeline_state = p_render_system.create_device_pipeline_state
		(
			"mesh_vs",
			nullptr,
			nullptr,
			nullptr,
			"mesh_ps",
			game::bc_vertex_type::pos_tex_nor_tan,
			game::bc_blend_type::opaque,
			game::bc_depth_stencil_type::depth_less_stencil_off,
			game::bc_rasterizer_type::fill_solid_cull_back,
			0x1,
			{ l_back_buffer_texture.get_format() },
			game::bc_surface_format_type::D32_FLOAT,
			game::bc_multi_sample_type::c1_q1
		);

		graphic::bc_device_parameters l_old_parameters
		(
			0,
			0,
			graphic::bc_format::unknown,
			graphic::bc_texture_ms_config(1, 0)
		);
		graphic::bc_device_parameters l_new_parameters
		(
			l_back_buffer_texture.get_width(),
			l_back_buffer_texture.get_height(),
			l_back_buffer_texture.get_format(),
			l_back_buffer_texture.get_sample_count()
		);

		after_reset(p_render_system, p_device, l_old_parameters, l_new_parameters);
	}

	void bc_back_buffer_output_pass::update(const game::bc_render_system_update_param& p_update_param)
	{
	}

	void bc_back_buffer_output_pass::initialize_frame(game::bc_render_system& p_render_system, game::bc_render_thread& p_thread)
	{
		p_thread.start(m_command_list.get());
		p_thread.bind_render_pass_state(m_render_pass_state.get());
	}

	void bc_back_buffer_output_pass::execute(game::bc_render_system& p_render_system, game::bc_render_thread& p_thread)
	{
		p_render_system.get_scene_graph().render_meshes(p_render_system, p_thread, false);

		p_thread.unbind_render_pass_state(m_render_pass_state.get());
		p_thread.finish();

		m_command_list->reset();
	}

	void bc_back_buffer_output_pass::before_reset(game::bc_render_system& p_render_system, graphic::bc_device& p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters)
	{
		if
		(
			p_old_parameters.m_width != p_new_parameters.m_width ||
			p_old_parameters.m_height != p_new_parameters.m_height
		)
		{
			m_render_pass_state.reset();
		}
	}

	void bc_back_buffer_output_pass::after_reset(game::bc_render_system& p_render_system, graphic::bc_device& p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters)
	{
		if
		(
			p_old_parameters.m_width != p_new_parameters.m_width ||
			p_old_parameters.m_height != p_new_parameters.m_height
		)
		{
			graphic::bc_texture2d l_back_buffer_texture = p_device.get_back_buffer_texture();

			auto l_linear_sampler_config = game::bc_graphic_state_configs::bc_sampler_config(game::bc_sampler_type::filter_linear_linear_linear_address_wrap_wrap_wrap);

			auto l_depth_stencil_view = *get_shared_resource< graphic::bc_depth_stencil_view >(game::bc_render_pass_resource_variable::depth_stencil_view);
			auto l_render_target_view = *get_shared_resource< graphic::bc_render_target_view >(game::bc_render_pass_resource_variable::render_target_view);
			auto l_viewport = graphic::bc_viewport::default_config(l_back_buffer_texture.get_width(), l_back_buffer_texture.get_height());
			auto l_linear_sampler = p_device.create_sampler_state(l_linear_sampler_config);

			m_render_pass_state = p_render_system.create_render_pass_state
			(
				m_pipeline_state.get(),
				l_viewport,
				{ graphic::bc_render_target_view_ptr(l_render_target_view) },
				l_depth_stencil_view,
				{ graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, l_linear_sampler) },
				{},
				{}
			);
		}
	}

	void bc_back_buffer_output_pass::destroy(graphic::bc_device& p_device)
	{
		m_command_list.reset();
		m_pipeline_state.reset();
		m_render_pass_state.reset();
	}

	core::bc_string bc_back_buffer_output_pass::get_name()
	{
		return "back_buffer_output_pass";
	}
}