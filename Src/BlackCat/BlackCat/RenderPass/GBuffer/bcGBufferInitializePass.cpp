// [03/07/2019 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Messaging/Query/bcQueryManager.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcWindComponent.h"
#include "Game/Query/bcMainCameraSceneQuery.h"
#include "BlackCat/RenderPass/GBuffer/bcGBufferInitializePass.h"
#include "BlackCat/bcConstant.h"

namespace black_cat
{
	void bc_gbuffer_initialize_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		const auto l_back_buffer_texture = l_device.get_back_buffer_texture();

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

		after_reset(game::bc_render_pass_reset_context(p_render_system, l_device, l_old_parameters, l_new_parameters));
	}

	void bc_gbuffer_initialize_pass::update(const game::bc_render_pass_update_context& p_update_param)
	{
	}

	void bc_gbuffer_initialize_pass::initialize_frame(const game::bc_render_pass_render_context& p_param)
	{
		auto* l_query_manager = core::bc_get_service< core::bc_query_manager >();
		const game::bc_camera_frustum l_frustum(p_param.m_update_camera);

		if(m_lights_query.is_executed())
		{
			m_lights_query_result = m_lights_query.get().get_lights();
		}
		if(m_winds_query.is_executed())
		{
			m_winds_query_result = m_winds_query.get().get_winds();
		}
		
		l_query_manager->queue_shared_query
		(
			game::bc_main_camera_scene_query(l_frustum)
		);
		m_lights_query = l_query_manager->queue_query
		(
			game::bc_scene_light_query(core::bc_enum::mask_or({ game::bc_light_type::direct }))
		);
		m_winds_query = l_query_manager->queue_query
		(
			game::bc_scene_wind_query()
		);
	}

	void bc_gbuffer_initialize_pass::execute(const game::bc_render_pass_render_context& p_param)
	{
		graphic::bc_render_target_view l_render_targets[] = { m_diffuse_map_view.get(), m_normal_map_view.get() };
		
		game::bc_direct_light l_direct_light({0,0,0}, {0,0,0}, 1, {0,0,0}, 1);
		game::bc_direct_wind l_direct_wind({0,0,0}, 1);

		const auto l_light_ite = std::max_element(std::begin(m_lights_query_result), std::end(m_lights_query_result), [](const game::bc_light_instance& p_light1, const game::bc_light_instance& p_light2)
		{
			return p_light1.as_direct_light()->get_intensity() < p_light2.as_direct_light()->get_intensity();
		});
		const auto l_wind_ite = std::max_element(std::begin(m_winds_query_result), std::end(m_winds_query_result), [](const game::bc_wind& p_wind1, const game::bc_wind& p_wind2)
		{
			return p_wind1.as_direct_wind()->m_power < p_wind2.as_direct_wind()->m_power;
		});

		if(l_light_ite != std::end(m_lights_query_result))
		{
			l_direct_light = *l_light_ite->as_direct_light();
		}
		if(l_wind_ite != std::end(m_winds_query_result))
		{
			l_direct_wind = *l_wind_ite->as_direct_wind();
		}
		
		p_param.m_render_thread.start();
		p_param.m_frame_renderer.update_global_cbuffer(p_param.m_render_thread, p_param.m_clock, p_param.m_render_camera, l_direct_light, l_direct_wind);
		
		p_param.m_render_thread.get_pipeline().bind_om_render_targets(2, &l_render_targets[0], m_depth_stencil_view.get());
		p_param.m_render_thread.get_pipeline().pipeline_apply_states(graphic::bc_pipeline_stage::output_merger_stage);
		
		p_param.m_render_thread.clear_buffers(core::bc_vector4f(0, 0, 255, 0), 1, 0);
		
		p_param.m_render_thread.get_pipeline().unbind_om_render_targets();
		p_param.m_render_thread.get_pipeline().pipeline_apply_states(graphic::bc_pipeline_stage::output_merger_stage);
		
		p_param.m_render_thread.finish();
	}

	void bc_gbuffer_initialize_pass::cleanup_frame(const game::bc_render_pass_render_context& p_param)
	{
	}

	void bc_gbuffer_initialize_pass::before_reset(const game::bc_render_pass_reset_context& p_param)
	{
		destroy(p_param.m_render_system);
	}

	void bc_gbuffer_initialize_pass::after_reset(const game::bc_render_pass_reset_context& p_param)
	{
		graphic::bc_graphic_resource_builder l_resource_configure;

		auto l_depth_stencil_config = l_resource_configure
			.as_resource()
			.as_texture2d
			(
				p_param.m_new_parameters.m_width,
				p_param.m_new_parameters.m_height,
				false,
				1,
				graphic::bc_format::D32_FLOAT,
				graphic::bc_resource_usage::gpu_rw,
				core::bc_enum::mask_or({ graphic::bc_resource_view_type::depth_stencil, graphic::bc_resource_view_type::shader })
			).as_depth_stencil_texture();
		auto l_diffuse_map_config = l_resource_configure
			.as_resource()
			.as_texture2d
			(
				p_param.m_new_parameters.m_width,
				p_param.m_new_parameters.m_height,
				false,
				1,
				graphic::bc_format::R8G8B8A8_UNORM,
				graphic::bc_resource_usage::gpu_rw,
				core::bc_enum::mask_or({ graphic::bc_resource_view_type::render_target, graphic::bc_resource_view_type::shader })
			).as_render_target_texture();
		auto l_normal_map_config = l_resource_configure
			.as_resource()
			.as_texture2d
			(
				p_param.m_new_parameters.m_width,
				p_param.m_new_parameters.m_height,
				false,
				1,
				graphic::bc_format::R8G8B8A8_UNORM,
				graphic::bc_resource_usage::gpu_rw,
				core::bc_enum::mask_or({ graphic::bc_resource_view_type::render_target, graphic::bc_resource_view_type::shader })
			).as_render_target_texture();

		auto l_depth_stencil_view_config = l_resource_configure
			.as_resource_view()
			.as_texture_view(graphic::bc_format::D32_FLOAT)
			.as_tex2d_depth_stencil_view(0);
		auto l_diffuse_map_view_config = l_resource_configure
			.as_resource_view()
			.as_texture_view(l_diffuse_map_config.get_format())
			.as_tex2d_render_target_view(0);
		auto l_normal_map_view_config = l_resource_configure
			.as_resource_view()
			.as_texture_view(l_normal_map_config.get_format())
			.as_tex2d_render_target_view(0);

		m_depth_stencil = p_param.m_device.create_texture2d(l_depth_stencil_config, nullptr);
		m_diffuse_map = p_param.m_device.create_texture2d(l_diffuse_map_config, nullptr);
		m_normal_map = p_param.m_device.create_texture2d(l_normal_map_config, nullptr);
		m_depth_stencil_view = p_param.m_device.create_depth_stencil_view(m_depth_stencil.get(), l_depth_stencil_view_config);
		m_diffuse_map_view = p_param.m_device.create_render_target_view(m_diffuse_map.get(), l_diffuse_map_view_config);
		m_normal_map_view = p_param.m_device.create_render_target_view(m_normal_map.get(), l_normal_map_view_config);

		share_resource(constant::g_rpass_depth_stencil_texture, m_depth_stencil.get());
		share_resource(constant::g_rpass_depth_stencil_render_view, m_depth_stencil_view.get());
		share_resource(constant::g_rpass_render_target_texture_1, m_diffuse_map.get());
		share_resource(constant::g_rpass_render_target_render_view_1, m_diffuse_map_view.get());
		share_resource(constant::g_rpass_render_target_texture_2, m_normal_map.get());
		share_resource(constant::g_rpass_render_target_render_view_2, m_normal_map_view.get());
	}

	void bc_gbuffer_initialize_pass::destroy(game::bc_render_system& p_render_system)
	{
		unshare_resource(constant::g_rpass_depth_stencil_texture);
		unshare_resource(constant::g_rpass_depth_stencil_render_view);
		unshare_resource(constant::g_rpass_render_target_texture_1);
		unshare_resource(constant::g_rpass_render_target_render_view_1);
		unshare_resource(constant::g_rpass_render_target_texture_2);
		unshare_resource(constant::g_rpass_render_target_render_view_2);

		m_depth_stencil_view.reset();
		m_diffuse_map.reset();
		m_normal_map.reset();
		m_depth_stencil.reset();
		m_diffuse_map.reset();
		m_normal_map.reset();
	}
}