// [08/30/2016 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Container/bcVector.h"
#include "Core/File/bcContentStreamManager.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/bcResourceConfig.h"
#include "Game/System/Render/bcVertexLayout.h"
#include "Game/Object/Mesh/bcHeightMap.h"
#include "Game/Object/Scence/Component/bcHeightMapComponent.h"
#include "BlackCat/bcException.h"
#include "BlackCat/RenderPass/bcTerrainPassDx11.h"
#include "BlackCat/Loader/bcHeightMapLoaderDx11.h"

namespace black_cat
{
	core::bc_vector4f _plane_from_3_point(const core::bc_vector3f& p_first,
		const core::bc_vector3f& p_second,
		const core::bc_vector3f& p_third)
	{
		auto l_vector1 = p_second - p_first;
		auto l_vector2 = p_third - p_first;

		auto l_normal = l_vector1.cross(l_vector2);
		l_normal.normalize();
		auto l_distance = l_normal.dot(p_first);

		return core::bc_vector4f(l_normal, -l_distance);
	}

	struct _bc_parameter_buffer
	{
		BC_CBUFFER_ALIGN
		core::bc_vector4f m_frustum_planes[6];
	};

	void bc_terrain_pass_dx11::initialize_resources(game::bc_render_system& p_render_system, graphic::bc_device* p_device)
	{
		/*auto* l_content_stream_manager = core::bc_service_manager::get().get_service< core::bc_content_stream_manager >();*/

		/*m_heightmap = l_content_stream_manager->find_content_throw< graphic::bc_texture2d_content >("heightmap");

		m_width = m_heightmap->get_resource()->get_width();
		m_height = m_heightmap->get_resource()->get_height();

		if(!_bc_is_power_of_two(m_width) || !_bc_is_power_of_two(m_height))
		{
			throw bc_invalid_operation_exception("Height map texture width and height must be power of 2");
		}*/

		//if(m_width < g_shader_thread_group_size || m_height < g_shader_thread_group_size) // thread group size in terrain tool shader is 32
		//{
		//	throw bc_invalid_operation_exception("Height map texture width and height must be greater than 32");
		//}

		/*m_position = graphic::bc_vector3f(0, 0, 0);
		m_chund_size = 64;
		m_xz_multiplier = 2;
		m_y_multiplier = 512;
		m_distance_detail = 100;
		m_height_detail = 20;*/

		graphic::bc_texture2d* l_back_buffer_texture = p_device->get_back_buffer_texture();

		m_command_list = p_device->create_command_list();
		m_pipeline_state = p_render_system.create_device_pipeline_state
		(
			"terrain_vs",
			"terrain_hs",
			"terrain_ds",
			nullptr,
			"terrain_ps",
			game::bc_vertex_type::pos_tex,
			game::bc_blend_type::opaque,
			game::bc_depth_stencil_type::depth_less_stencil_off,
			game::bc_rasterizer_type::fill_wireframe_cull_back,
			0x1,
			{ l_back_buffer_texture->get_format() },
			game::bc_surface_format_type::D32_FLOAT,
			game::bc_multi_sample_type::c1_q1
		);

		auto l_resource_configure = graphic::bc_graphic_resource_configure();

		auto l_parameter_cbuffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				1,
				sizeof(_bc_parameter_buffer),
				graphic::bc_resource_usage::gpu_rw
			)
			.as_constant_buffer();

		m_parameter_cbuffer = p_device->create_buffer(l_parameter_cbuffer_config, nullptr);

		graphic::bc_device_parameters l_old_parameters
		(
			0,
			0,
			graphic::bc_format::unknown,
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

		//bcUINT16 l_width_chunk_count = m_width / m_chund_size;
		//bcUINT16 l_height_chunk_count = m_height / m_chund_size;

		//core::bc_vector_frame< game::bc_vertex_pos_tex > l_vertices((l_width_chunk_count + 1) * (l_height_chunk_count + 1));
		//core::bc_vector_frame< bcUINT16 > l_indices(l_width_chunk_count * l_height_chunk_count * 4);
		//core::bc_vector_frame< std::array< bcINT32, 3 > > l_chunk_infoes(l_width_chunk_count * l_height_chunk_count);

		//bcUINT16 l_counter = 0;

		//for (bcINT16 z = -l_height_chunk_count / 2; z <= l_height_chunk_count / 2; ++z)
		//{
		//	for (bcINT16 x = -l_width_chunk_count / 2; x <= l_width_chunk_count / 2; ++x)
		//	{
		//		game::bc_vertex_pos_tex& l_vertex = l_vertices[l_counter++];

		//		l_vertex.m_position = graphic::bc_vector3f
		//		(
		//			x * m_chund_size * m_xz_multiplier + m_position.x,
		//			0,
		//			z * m_chund_size * m_xz_multiplier + m_position.z
		//		);
		//		l_vertex.m_texcoord = graphic::bc_vector2f
		//		(
		//			(static_cast< bcFLOAT >(x) + (l_width_chunk_count / 2)) / l_width_chunk_count,
		//			(static_cast< bcFLOAT >(z) + (l_height_chunk_count / 2)) / l_height_chunk_count
		//		);
		//	}
		//}

		//l_counter = 0;

		//for (bcUINT16 z = 0; z < l_height_chunk_count; z++)
		//{
		//	for (bcUINT16 x = 0; x < l_width_chunk_count; x++)
		//	{
		//		l_indices[l_counter++] = x + z * (l_width_chunk_count + 1);
		//		l_indices[l_counter++] = x + (z + 1) * (l_width_chunk_count + 1);
		//		l_indices[l_counter++] = (x + 1) + (z + 1) * (l_width_chunk_count + 1);
		//		l_indices[l_counter++] = (x + 1) + z * (l_width_chunk_count + 1);
		//		/*
		//		1-------2
		//		|		|
		//		|		|
		//		0-------3
		//		*/
		//	}
		//}

		//for (bcUINT16 x = 0, end = l_width_chunk_count * l_height_chunk_count; x < end; ++x)
		//{
		//	l_chunk_infoes[x] = std::array<bcINT32, 3> { std::numeric_limits<bcINT32>::max(), 0, 0 };
		//}

		//auto l_resource_configure = graphic::bc_graphic_resource_configure();

		//auto l_vbuffer_config = l_resource_configure
		//	.as_resource()
		//	.as_buffer
		//	(
		//		l_vertices.size(),
		//		sizeof(game::bc_vertex_pos_tex),
		//		graphic::bc_resource_usage::gpu_r
		//	)
		//	.as_vertex_buffer();
		//auto l_ibuffer_config = l_resource_configure
		//	.as_resource()
		//	.as_buffer
		//	(
		//		l_indices.size(),
		//		sizeof(bcUINT16),
		//		graphic::bc_resource_usage::gpu_r
		//	)
		//	.as_index_buffer();
		//auto l_chunk_info_buffer_config = l_resource_configure
		//	.as_resource()
		//	.as_buffer
		//	(
		//		l_width_chunk_count * l_height_chunk_count,
		//		sizeof(graphic::bc_vector3f),
		//		graphic::bc_resource_usage::gpu_rw,
		//		core::bc_enum::or({graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::unordered})
		//	)
		//	.as_structured_buffer(sizeof(graphic::bc_vector3f));
		//auto l_parameter_cbuffer_config = l_resource_configure
		//	.as_resource()
		//	.as_buffer
		//	(
		//		1,
		//		sizeof(_bc_parameter_buffer),
		//		graphic::bc_resource_usage::gpu_rw
		//	)
		//	.as_constant_buffer();

		//graphic::bc_subresource_data l_vbuffer_data(l_vertices.data(), 0, 0);
		//graphic::bc_subresource_data l_ibuffer_data(l_indices.data(), 0, 0);
		//graphic::bc_subresource_data l_chunk_info_buffer_data(l_chunk_infoes.data(), 0, 0);

		//m_vertex_buffer = p_device->create_buffer(l_vbuffer_config, &l_vbuffer_data);
		//m_index_buffer = p_device->create_buffer(l_ibuffer_config, &l_ibuffer_data);
		//m_chunk_info_buffer = p_device->create_buffer(l_chunk_info_buffer_config, &l_chunk_info_buffer_data);
		//m_parameter_cbuffer = p_device->create_buffer(l_parameter_cbuffer_config, nullptr);

		//auto l_chunk_info_view_config = l_resource_configure
		//	.as_resource_view()
		//	.as_buffer_view(graphic::bc_format::unknown)
		//	.as_shader_view(0, l_width_chunk_count * l_height_chunk_count)
		//	.as_structured_buffer();
		//auto l_chunk_info_unordered_view_config = l_resource_configure
		//	.as_resource_view()
		//	.as_buffer_view(graphic::bc_format::unknown)
		//	.as_unordered_view(0, l_width_chunk_count * l_height_chunk_count)
		//	.as_structured_buffer();
		//auto l_heightmap_view_config = l_resource_configure
		//	.as_resource_view()
		//	.as_texture_view(graphic::bc_format::R8G8B8A8_UNORM)
		//	.as_tex2d_shader_view(0, 1)
		//	.on_texture2d();

		//m_chunk_info_view = p_device->create_resource_view(m_chunk_info_buffer.get(), l_chunk_info_view_config);
		//m_chunk_info_unordered_view = p_device->create_resource_view(m_chunk_info_buffer.get(), l_chunk_info_unordered_view_config);
		//m_heightmap_view = p_device->create_resource_view(m_heightmap->get_resource().get(), l_heightmap_view_config);

		//m_render_state = p_render_system.create_render_state
		//(
		//	graphic::bc_primitive::control_point_4_patchlist,
		//	m_vertex_buffer,
		//	0,
		//	m_index_buffer,
		//	game::i16bit,
		//	l_indices.size(),
		//	0,
		//	{
		//		graphic::bc_resource_view_parameter(0, core::bc_enum::or({graphic::bc_shader_type::vertex, graphic::bc_shader_type::hull, graphic::bc_shader_type::domain, graphic::bc_shader_type::pixel}), m_heightmap_view),
		//		graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::hull, m_chunk_info_view)
		//	},
		//	{graphic::bc_constant_buffer_parameter(0, core::bc_enum::or({graphic::bc_shader_type::vertex, graphic::bc_shader_type::hull, graphic::bc_shader_type::domain, graphic::bc_shader_type::pixel}), m_parameter_cbuffer)}
		//);

		m_device_compute_state = p_render_system.create_device_compute_state("terrain_chunk_info");
		/*m_compute_state = p_render_system.create_compute_state
		(
			l_device_compute_state,
			m_width / s_shader_thread_group_size,
			m_height / s_shader_thread_group_size,
			1,
			{},
			{graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_heightmap_view)},
			{graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_chunk_info_unordered_view)},
			{graphic::bc_constant_buffer_parameter(0, graphic::bc_shader_type::compute, m_parameter_cbuffer)}
		);*/

		m_run_chunk_info_shader = true;
	}

	void bc_terrain_pass_dx11::update(const game::bc_render_system_update_param& p_update_param)
	{
		m_camera_extends = p_update_param.m_camera_extends;
	}

	void bc_terrain_pass_dx11::initialize_frame(game::bc_render_system& p_render_system, game::bc_render_thread& p_thread)
	{
		p_thread.start(m_command_list.get());

		_bc_parameter_buffer l_parameter;
		l_parameter.m_frustum_planes[0] = _plane_from_3_point(m_camera_extends[0], m_camera_extends[1], m_camera_extends[2]);
		l_parameter.m_frustum_planes[1] = _plane_from_3_point(m_camera_extends[6], m_camera_extends[5], m_camera_extends[4]);
		l_parameter.m_frustum_planes[2] = _plane_from_3_point(m_camera_extends[0], m_camera_extends[4], m_camera_extends[5]);
		l_parameter.m_frustum_planes[3] = _plane_from_3_point(m_camera_extends[1], m_camera_extends[5], m_camera_extends[6]);
		l_parameter.m_frustum_planes[4] = _plane_from_3_point(m_camera_extends[2], m_camera_extends[6], m_camera_extends[7]);
		l_parameter.m_frustum_planes[5] = _plane_from_3_point(m_camera_extends[7], m_camera_extends[4], m_camera_extends[0]);

		p_thread.update_subresource(m_parameter_cbuffer.get(), 0, &l_parameter, 0, 0);

		if(m_run_chunk_info_shader)
		{
			auto l_heightmaps = p_render_system.get_scence_graph().get_heightmaps();

			for(auto& l_actor : l_heightmaps)
			{
				auto& l_heightmap = l_actor.get_component< game::bc_height_map_component >()->get_height_map();
				auto& l_heightmap_dx11 = static_cast<bc_height_map_dx11&>(*l_heightmap);

				auto l_compute_state = p_render_system.create_compute_state
				(
					m_device_compute_state,
					l_heightmap_dx11.get_width() / s_shader_thread_group_size,
					l_heightmap_dx11.get_height() / s_shader_thread_group_size,
					1,
					{},
					{ graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, l_heightmap_dx11.get_height_map_view()) },
					{ graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, l_heightmap_dx11.get_chunk_info_unordered_view()) },
					{ graphic::bc_constant_buffer_parameter(0, graphic::bc_shader_type::compute, l_heightmap_dx11.get_parameter_cbuffer()) }
				);

				p_thread.run_compute_shader(l_compute_state.get());
			}

			m_run_chunk_info_shader = false;
		}

		p_thread.bind_render_pass_state(m_render_pass_state.get());

		p_thread.clear_buffers(core::bc_vector4f(0, 0, 255, 0), 1, 0);
	}

	void bc_terrain_pass_dx11::execute(game::bc_render_system& p_render_system, game::bc_render_thread& p_thread)
	{
		p_render_system.get_scence_graph().render_heightmaps(p_render_system);

		p_thread.unbind_render_pass_state(m_render_pass_state.get());
		p_thread.finish();

		m_command_list->reset();
	}

	void bc_terrain_pass_dx11::before_reset(game::bc_render_system& p_render_system, graphic::bc_device* p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters)
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

	void bc_terrain_pass_dx11::after_reset(game::bc_render_system& p_render_system, graphic::bc_device* p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters)
	{
		if
		(
			p_old_parameters.m_width != p_new_parameters.m_width ||
			p_old_parameters.m_height != p_new_parameters.m_height
		)
		{
			graphic::bc_texture2d* l_back_buffer_texture = p_device->get_back_buffer_texture();

			auto l_linear_sampler_config = game::bc_graphic_state_configs::bc_sampler_config(game::bc_sampler_type::filter_linear_linear_linear_address_clamp_clamp_clamp);

			auto* l_depth_stencil_view = get_shared_resource< graphic::bc_depth_stencil_view_ptr >(game::bc_render_pass_resource_variable::depth_stencil_view);
			auto* l_render_target_view = get_shared_resource< graphic::bc_render_target_view_ptr >(game::bc_render_pass_resource_variable::render_target_view);
			auto l_viewport = graphic::bc_viewport::default_config(l_back_buffer_texture->get_width(), l_back_buffer_texture->get_height());
			auto l_linear_sampler = p_device->create_sampler_state(l_linear_sampler_config);

			m_render_pass_state = p_render_system.create_render_pass_state
			(
				m_pipeline_state,
				l_viewport,
				{*l_render_target_view},
				*l_depth_stencil_view,
				{
					graphic::bc_sampler_parameter(0, core::bc_enum::or({graphic::bc_shader_type::vertex, graphic::bc_shader_type::domain, graphic::bc_shader_type::pixel}), l_linear_sampler)},
				{},
				{
					graphic::bc_constant_buffer_parameter(0, graphic::bc_shader_type::hull, m_parameter_cbuffer)
				}
			);
		}
	}

	void bc_terrain_pass_dx11::destroy(graphic::bc_device* p_device)
	{
		m_device_compute_state.reset();
		m_render_pass_state.reset();
		m_parameter_cbuffer.reset();
		m_pipeline_state.reset();
		m_command_list.reset();
	}

	core::bc_string bc_terrain_pass_dx11::get_name()
	{
		return "terrain_pass";
	}
}
