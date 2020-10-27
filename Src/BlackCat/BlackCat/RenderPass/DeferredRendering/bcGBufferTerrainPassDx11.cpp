// [08/30/2016 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Container/bcVector.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/State/bcVertexLayout.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "Game/Object/Mesh/bcHeightMap.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/bcQuery.h"
#include "BlackCat/RenderPass/DeferredRendering/bcGBufferTerrainPassDx11.h"
#include "BlackCat/Loader/bcHeightMapLoaderDx11.h"
#include "BlackCat/bcException.h"

namespace black_cat
{
	core::bc_vector4f _plane_from_3_point(const core::bc_vector3f& p_first,
		const core::bc_vector3f& p_second,
		const core::bc_vector3f& p_third)
	{
		const auto l_vector1 = p_second - p_first;
		const auto l_vector2 = p_third - p_first;

		auto l_normal = l_vector1.cross(l_vector2);
		l_normal.normalize();
		const auto l_distance = l_normal.dot(p_first);

		return core::bc_vector4f(l_normal, -l_distance);
	}

	struct _bc_parameter_buffer
	{
		BC_CBUFFER_ALIGN
		core::bc_vector4f m_frustum_planes[6];
	};

	void bc_gbuffer_terrain_pass_dx11::initialize_resources(game::bc_render_system& p_render_system)
	{
		graphic::bc_device& l_device = p_render_system.get_device();
		graphic::bc_texture2d l_back_buffer_texture = l_device.get_back_buffer_texture();

		m_command_list = l_device.create_command_list();
		m_pipeline_state = p_render_system.create_device_pipeline_state
		(
			"terrain_vs",
			"terrain_hs",
			"terrain_ds",
			nullptr,
			"terrain_gbuffer_ps",
			game::bc_vertex_type::pos_tex,
			game::bc_blend_type::opaque,
			game::bc_depth_stencil_type::depth_less_stencil_off,
			game::bc_rasterizer_type::fill_solid_cull_back,
			0x1,
			{
				get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_1)->get_format(),
				get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_2)->get_format()
			},
			get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_depth_stencil_texture)->get_format(),
			game::bc_multi_sample_type::c1_q1
		);

		auto l_resource_configure = graphic::bc_graphic_resource_builder();

		auto l_parameter_cbuffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				1,
				sizeof(_bc_parameter_buffer),
				graphic::bc_resource_usage::gpu_rw
			)
			.as_constant_buffer();

		m_parameter_cbuffer = l_device.create_buffer(l_parameter_cbuffer_config, nullptr);

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

		m_device_compute_state = p_render_system.create_device_compute_state("terrain_chunk_info");
		m_run_chunk_info_shader = true;
	}

	void bc_gbuffer_terrain_pass_dx11::update(const game::bc_render_pass_update_param& p_update_param)
	{
	}

	void bc_gbuffer_terrain_pass_dx11::initialize_frame(const game::bc_render_pass_render_param& p_param)
	{
		if(m_scene_query.is_executed())
		{
			m_scene_query_result = m_scene_query.get().get_scene_buffer();
		}

		m_scene_query = core::bc_get_service<core::bc_query_manager>()->queue_query
		(
			game::bc_scene_graph_query().only<game::bc_height_map_component>()
		);
		
		if (m_run_chunk_info_shader)
		{
			if(m_scene_query_result.size() == 0)
			{
				return;
			}
			
			p_param.m_render_thread.start(m_command_list.get());

			for (auto& l_actor : m_scene_query_result)
			{
				const game::bc_height_map& l_height_map = l_actor.get_component< game::bc_height_map_component >()->get_height_map();
				const bc_height_map_dx11& l_height_map_dx11 = static_cast<const bc_height_map_dx11&>(l_height_map);

				auto l_compute_state = p_param.m_render_system.create_compute_state
				(
					m_device_compute_state.get(),
					l_height_map_dx11.get_width() / s_shader_thread_group_size,
					l_height_map_dx11.get_height() / s_shader_thread_group_size,
					1,
					{},
					{ graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, l_height_map_dx11.get_height_map_view()) },
					{ graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, l_height_map_dx11.get_chunk_info_unordered_view()) },
					{ graphic::bc_constant_buffer_parameter(0, graphic::bc_shader_type::compute, l_height_map_dx11.get_parameter_cbuffer()) }
				);

				p_param.m_render_thread.run_compute_shader(*l_compute_state.get());
			}

			p_param.m_render_thread.finish();
			m_run_chunk_info_shader = false;
		}
	}

	void bc_gbuffer_terrain_pass_dx11::execute(const game::bc_render_pass_render_param& p_param)
	{
		if(m_scene_query_result.size() == 0)
		{
			return;
		}
		
		game::bc_icamera::extend l_camera_extends = p_param.m_camera.get_extends();

		_bc_parameter_buffer l_parameter;
		l_parameter.m_frustum_planes[0] = _plane_from_3_point(l_camera_extends[0], l_camera_extends[1], l_camera_extends[2]);
		l_parameter.m_frustum_planes[1] = _plane_from_3_point(l_camera_extends[6], l_camera_extends[5], l_camera_extends[4]);
		l_parameter.m_frustum_planes[2] = _plane_from_3_point(l_camera_extends[0], l_camera_extends[4], l_camera_extends[5]);
		l_parameter.m_frustum_planes[3] = _plane_from_3_point(l_camera_extends[1], l_camera_extends[5], l_camera_extends[6]);
		l_parameter.m_frustum_planes[4] = _plane_from_3_point(l_camera_extends[2], l_camera_extends[6], l_camera_extends[7]);
		l_parameter.m_frustum_planes[5] = _plane_from_3_point(l_camera_extends[7], l_camera_extends[4], l_camera_extends[0]);
		
		p_param.m_render_thread.start(m_command_list.get());

		p_param.m_frame_renderer.update_global_cbuffer(p_param.m_render_thread, p_param.m_clock, p_param.m_camera);
		p_param.m_render_thread.update_subresource(m_parameter_cbuffer.get(), 0, &l_parameter, 0, 0);
		
		p_param.m_render_thread.bind_render_pass_state(*m_render_pass_state.get());
		p_param.m_render_thread.clear_buffers(core::bc_vector4f(0, 0, 255, 0), 1, 0);

		auto l_render_state_buffer = p_param.m_frame_renderer.create_buffer();
		m_scene_query_result.render_actors(l_render_state_buffer);

		p_param.m_frame_renderer.render_buffer(l_render_state_buffer, p_param.m_render_thread, p_param.m_camera);

		p_param.m_render_thread.unbind_render_pass_state(*m_render_pass_state.get());
		p_param.m_render_thread.finish();

		m_command_list->finished();
	}

	void bc_gbuffer_terrain_pass_dx11::cleanup_frame(const game::bc_render_pass_render_param& p_param)
	{
	}

	void bc_gbuffer_terrain_pass_dx11::before_reset(const game::bc_render_pass_reset_param& p_param)
	{
		if
		(
			p_param.m_old_parameters.m_width != p_param.m_new_parameters.m_width ||
			p_param.m_old_parameters.m_height != p_param.m_new_parameters.m_height
		)
		{
			m_render_pass_state.reset();
		}
	}

	void bc_gbuffer_terrain_pass_dx11::after_reset(const game::bc_render_pass_reset_param& p_param)
	{
		if
		(
			p_param.m_old_parameters.m_width != p_param.m_new_parameters.m_width ||
			p_param.m_old_parameters.m_height != p_param.m_new_parameters.m_height
		)
		{
			graphic::bc_texture2d l_back_buffer_texture = p_param.m_device.get_back_buffer_texture();

			const auto l_height_map_sampler_config = graphic::bc_graphic_resource_builder().as_resource().as_sampler_state
			(
				graphic::bc_filter::min_mag_mip_linear,
				graphic::bc_texture_address_mode::clamp,
				graphic::bc_texture_address_mode::clamp,
				graphic::bc_texture_address_mode::clamp
			).as_sampler_state();
			const auto l_texture_sampler_config = graphic::bc_graphic_resource_builder().as_resource().as_sampler_state
			(
				graphic::bc_filter::min_mag_mip_linear,
				graphic::bc_texture_address_mode::wrap,
				graphic::bc_texture_address_mode::wrap,
				graphic::bc_texture_address_mode::wrap
			).as_sampler_state();
			const auto* l_depth_stencil_view = get_shared_resource<graphic::bc_depth_stencil_view>(constant::g_rpass_depth_stencil_view);
			const auto* l_diffuse_map_view = get_shared_resource<graphic::bc_render_target_view>(constant::g_rpass_render_target_view_1);
			const auto* l_normal_map_view = get_shared_resource<graphic::bc_render_target_view>(constant::g_rpass_render_target_view_2);
			const auto l_viewport = graphic::bc_viewport::default_config(p_param.m_device.get_back_buffer_width(), p_param.m_device.get_back_buffer_height());

			m_height_map_sampler = p_param.m_device.create_sampler_state(l_height_map_sampler_config);
			m_texture_sampler = p_param.m_device.create_sampler_state(l_texture_sampler_config);
			m_render_pass_state = p_param.m_render_system.create_render_pass_state
			(
				m_pipeline_state.get(),
				l_viewport,
				{ *l_diffuse_map_view, *l_normal_map_view },
				*l_depth_stencil_view,
				{
					graphic::bc_sampler_parameter(0, core::bc_enum:: or ({ graphic::bc_shader_type::pixel }), m_height_map_sampler.get()),
					graphic::bc_sampler_parameter(0, core::bc_enum:: or ({ graphic::bc_shader_type::pixel }), m_texture_sampler.get())
				},
				{},
				{
					p_param.m_render_system.get_global_cbuffer(),
					graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::hull, m_parameter_cbuffer.get())
				}
			);
		}
	}

	void bc_gbuffer_terrain_pass_dx11::destroy(game::bc_render_system& p_render_system)
	{
		m_device_compute_state.reset();
		m_pipeline_state.reset();
		m_render_pass_state.reset();
		m_parameter_cbuffer.reset();
		m_height_map_sampler.reset();
		m_texture_sampler.reset();
		m_command_list.reset();
	}

	void bc_gbuffer_terrain_pass_dx11::update_chunk_infos()
	{
		m_run_chunk_info_shader = true;
	}
}