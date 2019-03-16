// [08/31/2016 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "GraphicImp/Resource/bcResourceConfig.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "Game/Object/Scene/bcScene.h"
#include "BlackCat/RenderPass/bcInitializePass.h"

namespace black_cat
{
	void bc_initialize_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		m_depth_stencil_format = graphic::bc_format::D32_FLOAT;

		graphic::bc_texture2d l_back_buffer_texture = l_device.get_back_buffer_texture();

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

		after_reset(game::bc_render_pass_reset_param(p_render_system, l_device, l_old_parameters, l_new_parameters));
	}

	void bc_initialize_pass::update(const game::bc_render_pass_update_param& p_update_param)
	{
	}

	void bc_initialize_pass::initialize_frame(const game::bc_render_pass_render_param& p_param)
	{
		game::bc_camera_frustum l_frustum(p_param.m_camera);
		game::bc_scene_graph_buffer l_actors = p_param.m_scene.get_actors(l_frustum);

		share_resource(game::bc_render_pass_resource_variable::actor_list, std::move(l_actors));
	}

	void bc_initialize_pass::execute(const game::bc_render_pass_render_param& p_param)
	{
	}

	void bc_initialize_pass::cleanup_frame(const game::bc_render_pass_render_param& p_param)
	{
		unshare_resource(game::bc_render_pass_resource_variable::actor_list);
	}

	void bc_initialize_pass::before_reset(const game::bc_render_pass_reset_param& p_param)
	{
		destroy(p_param.m_render_system);
	}

	void bc_initialize_pass::after_reset(const game::bc_render_pass_reset_param& p_param)
	{
		graphic::bc_graphic_resource_configure l_resource_configure;

		m_render_target_format = p_param.m_new_parameters.m_format;

		auto l_back_buffer_texture = p_param.m_device.get_back_buffer_texture();
		auto l_depth_buffer_config = l_resource_configure.as_resource().as_texture2d
			(
				p_param.m_new_parameters.m_width,
				p_param.m_new_parameters.m_height,
				false,
				1,
				m_depth_stencil_format,
				graphic::bc_resource_usage::gpu_rw,
				graphic::bc_resource_view_type::depth_stencil
			)
			.as_depth_stencil_texture();
		auto l_depth_view_config = l_resource_configure
			.as_resource_view()
			.as_texture_view(graphic::bc_format::D32_FLOAT)
			.as_tex2d_depth_stencil_view(0);
		auto l_render_target_config = l_resource_configure
			.as_resource_view()
			.as_texture_view(m_render_target_format)
			.as_tex2d_render_target_view(0);

		m_depth_buffer = p_param.m_device.create_texture2d(l_depth_buffer_config, nullptr);
		m_depth_stencil_view = p_param.m_device.create_depth_stencil_view(m_depth_buffer.get(), l_depth_view_config);
		m_render_target_view = p_param.m_device.create_render_target_view(l_back_buffer_texture, l_render_target_config);

		share_resource(game::bc_render_pass_resource_variable::depth_stencil_texture, m_depth_buffer.get());
		share_resource(game::bc_render_pass_resource_variable::depth_stencil_view, m_depth_stencil_view.get());
		share_resource(game::bc_render_pass_resource_variable::render_target_view_1, m_render_target_view.get());
	}

	void bc_initialize_pass::destroy(game::bc_render_system& p_render_system)
	{
		unshare_resource(game::bc_render_pass_resource_variable::depth_stencil_texture);
		unshare_resource(game::bc_render_pass_resource_variable::depth_stencil_view);
		unshare_resource(game::bc_render_pass_resource_variable::render_target_view_1);

		m_depth_buffer.reset();
		m_depth_stencil_view.reset();
		m_render_target_view.reset();
	}
}