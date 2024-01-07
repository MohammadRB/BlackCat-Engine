// [30/08/2016 MRB]

#include "App/AppPCH.h"

#include "Core/Container/bcVector.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/State/bcVertexLayout.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/Object/Mesh/bcHeightMap.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "App/RenderPass/GBuffer/bcGBufferTerrainPassDx11.h"
#include "App/Loader/bcHeightMapLoaderDx11.h"
#include "App/bcException.h"
#include "App/bcConstant.h"

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
		auto& l_device = p_render_system.get_device();
		auto& l_device_swap_buffer = p_render_system.get_device_swap_buffer();

		const auto l_parameter_cbuffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer
			(
				1,
				sizeof(_bc_parameter_buffer),
				graphic::bc_resource_usage::gpu_rw
			)
			.as_constant_buffer();
		
		m_parameter_cbuffer = l_device.create_buffer(l_parameter_cbuffer_config, nullptr);
		m_chunk_info_device_compute_state = p_render_system.create_device_compute_state("terrain_chunk_info_cs");
		m_run_chunk_info_shader = true;
		
		after_reset(game::bc_render_pass_reset_context
		(
			p_render_system,
			l_device,
			l_device_swap_buffer,
			graphic::bc_device_parameters
			(
				0,
				0,
				graphic::bc_format::unknown,
				graphic::bc_texture_ms_config(1, 0)
			),
			graphic::bc_device_parameters
			(
				l_device_swap_buffer.get_back_buffer_width(),
				l_device_swap_buffer.get_back_buffer_height(),
				l_device_swap_buffer.get_back_buffer_format(),
				l_device_swap_buffer.get_back_buffer_texture().get_sample_count()
			)
		));
	}

	void bc_gbuffer_terrain_pass_dx11::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_gbuffer_terrain_pass_dx11::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
		core::bc_vector<game::bc_height_map_ptr> l_height_maps;

		if (m_height_maps_query_result.is_executed())
		{
			l_height_maps = m_height_maps_query.get_height_maps();
			m_height_maps_render_buffer = m_height_maps_query.get_render_state_buffer();
		}

		m_height_maps_query = game::bc_height_map_scene_query(game::bc_actor_render_camera(p_context.m_update_camera), p_context.m_frame_renderer.create_buffer());
		m_height_maps_query_result = p_context.m_query_manager.queue_query_ref(m_height_maps_query);

		const auto l_height_maps_changed = std::any_of
		(
			std::begin(l_height_maps),
			std::end(l_height_maps),
			[&](const game::bc_height_map_ptr& p_height_map)
			{
				return std::find
				(
					std::begin(m_last_frame_height_maps),
					std::end(m_last_frame_height_maps),
					p_height_map.get()
				) == std::end(m_last_frame_height_maps);
			}
		);

		if (m_run_chunk_info_shader || l_height_maps_changed)
		{
			if(l_height_maps.empty())
			{
				return;
			}
			
			p_context.m_render_thread.start();

			for (auto& l_height_map : l_height_maps)
			{
				const auto& l_height_map_dx11 = static_cast<const bc_height_map_dx11&>(*l_height_map);

				auto l_compute_state = p_context.m_render_system.create_compute_state
				(
					m_chunk_info_device_compute_state.get(),
					{},
					{ graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, l_height_map_dx11.get_height_map_view()) },
					{ graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, l_height_map_dx11.get_chunk_info_unordered_view()) },
					{ graphic::bc_constant_buffer_parameter(0, graphic::bc_shader_type::compute, l_height_map_dx11.get_parameter_cbuffer()) }
				);

				p_context.m_render_thread.bind_compute_state(*l_compute_state);
				p_context.m_render_thread.dispatch
				(
					l_height_map_dx11.get_width() / s_shader_thread_group_size,
					l_height_map_dx11.get_height() / s_shader_thread_group_size,
					1
				);
				p_context.m_render_thread.unbind_compute_state(*l_compute_state);
			}

			p_context.m_render_thread.finish();
			m_run_chunk_info_shader = false;
		}

		m_last_frame_height_maps.clear();
		std::transform
		(
			std::begin(l_height_maps),
			std::end(l_height_maps),
			std::back_inserter(m_last_frame_height_maps),
			[](const game::bc_height_map_ptr& p_height_map)
			{
				return p_height_map.get();
			}
		);
	}

	void bc_gbuffer_terrain_pass_dx11::execute(const game::bc_render_pass_render_context& p_context)
	{
		const auto l_camera_extends = p_context.m_render_camera.get_extend_points();

		_bc_parameter_buffer l_parameter;
		l_parameter.m_frustum_planes[0] = _plane_from_3_point(l_camera_extends[0], l_camera_extends[1], l_camera_extends[2]);
		l_parameter.m_frustum_planes[1] = _plane_from_3_point(l_camera_extends[6], l_camera_extends[5], l_camera_extends[4]);
		l_parameter.m_frustum_planes[2] = _plane_from_3_point(l_camera_extends[0], l_camera_extends[4], l_camera_extends[5]);
		l_parameter.m_frustum_planes[3] = _plane_from_3_point(l_camera_extends[1], l_camera_extends[5], l_camera_extends[6]);
		l_parameter.m_frustum_planes[4] = _plane_from_3_point(l_camera_extends[2], l_camera_extends[6], l_camera_extends[7]);
		l_parameter.m_frustum_planes[5] = _plane_from_3_point(l_camera_extends[7], l_camera_extends[4], l_camera_extends[0]);
		
		p_context.m_render_thread.start();
		p_context.m_render_thread.bind_render_pass_state(*m_render_pass_state.get());

		p_context.m_render_thread.update_subresource(m_parameter_cbuffer.get(), 0, &l_parameter, 0, 0);
		
		p_context.m_frame_renderer.render_buffer(p_context.m_render_thread, m_height_maps_render_buffer, p_context.m_render_camera);

		p_context.m_render_thread.unbind_render_pass_state(*m_render_pass_state.get());
		p_context.m_render_thread.finish();
	}

	void bc_gbuffer_terrain_pass_dx11::cleanup_frame(const game::bc_render_pass_render_context& p_context)
	{
		m_height_maps_render_buffer = game::bc_render_state_buffer();
	}

	void bc_gbuffer_terrain_pass_dx11::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
		m_render_pass_state.reset();
	}

	void bc_gbuffer_terrain_pass_dx11::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		graphic::bc_texture2d l_back_buffer_texture = p_context.m_device_swap_buffer.get_back_buffer_texture();

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

		const auto* l_depth_stencil = get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_depth_stencil_texture);
		const auto* l_diffuse_map = get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_1);
		const auto* l_normal_map = get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_2);
		const auto* l_specular_map = get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_3);
		const auto* l_depth_stencil_view = get_shared_resource<graphic::bc_depth_stencil_view>(constant::g_rpass_depth_stencil_render_view);
		const auto* l_diffuse_map_view = get_shared_resource<graphic::bc_render_target_view>(constant::g_rpass_render_target_render_view_1);
		const auto* l_normal_map_view = get_shared_resource<graphic::bc_render_target_view>(constant::g_rpass_render_target_render_view_2);
		const auto* l_specular_map_view = get_shared_resource<graphic::bc_render_target_view>(constant::g_rpass_render_target_render_view_3);
		const auto l_viewport = graphic::bc_viewport::default_config(p_context.m_device_swap_buffer.get_back_buffer_width(), p_context.m_device_swap_buffer.get_back_buffer_height());

		m_height_map_sampler = p_context.m_device.create_sampler_state(l_height_map_sampler_config);
		m_texture_sampler = p_context.m_device.create_sampler_state(l_texture_sampler_config);
		m_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"terrain_vs",
			"terrain_hs",
			"terrain_ds",
			nullptr,
			"terrain_gbuffer_ps",
			game::bc_vertex_type::pos_tex,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_less, game::bc_depth_stencil_type::stencil_write }),
			game::bc_rasterizer_type::fill_solid_cull_back,
			0x1,
			{
				l_diffuse_map->get_format(),
				l_normal_map->get_format(),
				l_specular_map->get_format()
			},
			l_depth_stencil->get_format(),
			game::bc_multi_sample_type::c1_q1
		);
		m_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			m_device_pipeline_state.get(),
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(*l_diffuse_map_view),
				graphic::bc_render_target_view_parameter(*l_normal_map_view),
				graphic::bc_render_target_view_parameter(*l_specular_map_view)
			},
			*l_depth_stencil_view,
			{
				graphic::bc_sampler_parameter(0, core::bc_enum::mask_or({ graphic::bc_shader_type::pixel }), m_height_map_sampler.get()),
				graphic::bc_sampler_parameter(0, core::bc_enum::mask_or({ graphic::bc_shader_type::pixel }), m_texture_sampler.get())
			},
			{},
			{},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::hull, m_parameter_cbuffer.get())
			}
		);
	}

	void bc_gbuffer_terrain_pass_dx11::destroy(game::bc_render_system& p_render_system)
	{
		m_device_pipeline_state.reset();
		m_chunk_info_device_compute_state.reset();
		m_parameter_cbuffer.reset();
		m_height_map_sampler.reset();
		m_texture_sampler.reset();
		m_render_pass_state.reset();
	}

	void bc_gbuffer_terrain_pass_dx11::update_chunk_infos()
	{
		m_run_chunk_info_shader = true;
	}
}