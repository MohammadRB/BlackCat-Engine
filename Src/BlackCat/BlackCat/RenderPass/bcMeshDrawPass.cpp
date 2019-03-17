// [03/10/2016 MRB]

#include "BlackCat/BlackCatPCH.h"
#include "Core/Utility/bcEnumOperand.h"
#include "GraphicImp/Resource/bcResourceConfig.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecutor.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/Pass/bcRenderPassResourceShare.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "BlackCat/RenderPass/bcMeshDrawPass.h"

namespace black_cat
{
	void bc_mesh_draw_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		graphic::bc_texture2d l_back_buffer_texture = l_device.get_back_buffer_texture();

		m_command_list = l_device.create_command_list();
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

		after_reset(p_render_system, l_device, l_old_parameters, l_new_parameters);
	}

	void bc_mesh_draw_pass::update(const game::bc_render_system_update_param& p_update_param)
	{
	}

	void bc_mesh_draw_pass::initialize_frame(game::bc_render_system& p_render_system, game::bc_render_thread& p_thread, game::bc_scene& p_scene)
	{
		p_thread.start(m_command_list.get());
		p_thread.bind_render_pass_state(m_render_pass_state.get());
	}

	void bc_mesh_draw_pass::execute(game::bc_render_system& p_render_system, game::bc_render_thread& p_thread, game::bc_scene& p_scene)
	{
		game::bc_scene_graph_buffer* l_actors = get_shared_resource<game::bc_scene_graph_buffer>(game::bc_render_pass_resource_variable::actor_list);

		l_actors->render_actors<game::bc_mesh_component>(p_render_system);
		p_render_system.render_all_instances(p_thread);
		p_render_system.clear_render_instances();

		p_thread.unbind_render_pass_state(m_render_pass_state.get());
		p_thread.finish();

		m_command_list->reset();
	}

	void bc_mesh_draw_pass::before_reset(game::bc_render_system& p_render_system, graphic::bc_device& p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters)
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

	void bc_mesh_draw_pass::after_reset(game::bc_render_system& p_render_system, graphic::bc_device& p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters)
	{
		if
		(
			p_old_parameters.m_width != p_new_parameters.m_width ||
			p_old_parameters.m_height != p_new_parameters.m_height
		)
		{
			graphic::bc_texture2d l_back_buffer_texture = p_device.get_back_buffer_texture();

			auto l_linear_sampler_config = game::bc_graphic_state_configs::bc_sampler_config(game::bc_sampler_type::filter_linear_linear_linear_address_wrap_wrap_wrap);

			const auto l_depth_stencil_view = *get_shared_resource< graphic::bc_depth_stencil_view >(game::bc_render_pass_resource_variable::depth_stencil_view);
			const auto l_render_target_view = *get_shared_resource< graphic::bc_render_target_view >(game::bc_render_pass_resource_variable::render_target_view);
			const auto l_viewport = graphic::bc_viewport::default_config(l_back_buffer_texture.get_width(), l_back_buffer_texture.get_height());
			m_sampler_state = p_device.create_sampler_state(l_linear_sampler_config);

			m_render_pass_state = p_render_system.create_render_pass_state
			(
				m_pipeline_state.get(),
				l_viewport,
				{ l_render_target_view },
				l_depth_stencil_view,
				{ graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_sampler_state.get()) },
				{},
				{}
			);
		}
	}

	void bc_mesh_draw_pass::destroy(graphic::bc_device& p_device)
	{
		m_command_list.reset();
		m_pipeline_state.reset();
		m_sampler_state.reset();
		m_render_pass_state.reset();
	}
}