// [09/13/2016 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Utility/bcUtility.h"
#include "Core/File/bcContentManager.h"
#include "Core/Math/bcVector3f.h"
#include "GraphicImp/Resource/bcResourceConfig.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "PhysicsImp/Fundation/bcMeshBuffer.h"
#include "PhysicsImp/Shape/bcHeightField.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Render/bcVertexLayout.h"
#include "Game/System/Render/bcRenderTask.h"
#include "Game/Object/Mesh/bcHeightMap.h"
#include "BlackCat/Loader/bcHeightMapLoaderDx11.h"

namespace black_cat
{
	struct _bc_parameter_buffer
	{
		BC_CBUFFER_ALIGN
		bcUINT32 m_width;
		bcUINT32 m_height;
		bcUINT32 m_chunk_size;
		bcUINT32 m_xz_multiplier;
		BC_CBUFFER_ALIGN
		bcUINT32 m_y_multiplier;
		bcUINT32 m_distance_detail;
		bcUINT32 m_height_detail;
	};

	class bc_height_map_texture_read_task : public game::bc_irender_task
	{
	public:
		explicit bc_height_map_texture_read_task(graphic::bc_texture2d p_height_map)
			: m_height_map(p_height_map)
		{
		}

		void execute(game::bc_render_system& p_render_system, game::bc_render_thread& p_render_thread) override
		{
			auto& l_device = p_render_system.get_device();
			auto l_width = m_height_map.get_width();
			auto l_height = m_height_map.get_height();

			bcUINT32 k_buffer_size = l_width * l_height * 4;
			auto l_texture_config = graphic::bc_graphic_resource_configure().as_resource()
				.as_texture2d
				(
					l_width,
					l_height,
					false,
					1,
					graphic::bc_format::R8G8B8A8_UNORM,
					graphic::bc_resource_usage::gpu_r_cpu_r,
					graphic::bc_resource_view_type::none
				)
				.as_normal_texture();
			auto l_texture = l_device.create_texture2d(l_texture_config, nullptr);
			auto l_command_list = l_device.create_command_list();

			p_render_thread.start(l_command_list.get());

			p_render_thread.copy_resource(l_texture.get(), m_height_map);

			p_render_thread.finish();

			m_texure_buffer.reset(static_cast< bcUBYTE* >(bcAlloc(k_buffer_size, core::bc_alloc_type::frame)));

			auto l_mapped = l_device.map_resource(l_texture.get(), 0, graphic::bc_resource_map::read);

			std::memcpy(m_texure_buffer.get(), l_mapped.m_data, k_buffer_size);

			l_device.unmap_resource(l_texture.get(), 0);
		}

		graphic::bc_texture2d m_height_map;
		core::bc_unique_ptr<bcUBYTE> m_texure_buffer;
	};

	bc_height_map_dx11::bc_height_map_dx11(const core::bc_vector3f& p_position,
		bcUINT16 p_xz_multiplier,
		bcUINT16 p_y_multiplier,
		bcUINT16 p_distance_detail,
		bcUINT16 p_height_detail,
		graphic::bc_texture2d_content_ptr& p_height_map,
		graphic::bc_resource_view_ptr& p_height_map_view,
		graphic::bc_buffer_ptr& p_parameter_cbuffer,
		game::bc_render_state_ptr& p_render_state,
		graphic::bc_buffer_ptr& p_chunk_info_buffer,
		graphic::bc_resource_view_ptr& p_chunk_info_view,
		graphic::bc_resource_view_ptr& p_chunk_info_unordered_view,
		physics::bc_height_field_ref&& p_px_height_map)
		: bc_height_map
		(
			p_position,
			p_height_map->get_resource()->get_width(),
			p_height_map->get_resource()->get_height(),
			p_xz_multiplier,
			p_y_multiplier,
			p_render_state,
			std::move(p_px_height_map)
		),
		m_distance_detail(p_distance_detail),
		m_height_detail(p_height_detail),
		m_heightmap(p_height_map),
		m_height_map_view(p_height_map_view),
		m_parameter_cbuffer(p_parameter_cbuffer),
		m_chunk_info_buffer(p_chunk_info_buffer),
		m_chunk_info_view(p_chunk_info_view),
		m_chunk_info_unordered_view(p_chunk_info_unordered_view)
	{
	}

	void bc_height_map_loader_dx11::content_offline_processing(core::bc_content_loader_context& p_context)
	{
		auto* l_content_loader = core::bc_get_service< core::bc_content_manager >();
		auto* l_game_system = core::bc_get_service< game::bc_game_system >();
		auto& l_render_system = l_game_system->get_render_system();
		auto& l_physics_system = l_game_system->get_physics_system();

		auto l_heightmap_texture = l_content_loader->load< graphic::bc_texture2d_content >
		(
			p_context.get_allocator_alloc_type(),
			p_context.m_file_path.c_str(),
			core::bc_content_loader_parameter(p_context.m_parameter)
		);

		auto l_width = l_heightmap_texture->get_resource()->get_width();
		auto l_height = l_heightmap_texture->get_resource()->get_height();

		if (!bc_is_power_of_two(l_width) || !bc_is_power_of_two(l_height))
		{
			throw bc_invalid_operation_exception("Height map texture width and height must be power of 2");
		}

		if (l_width < s_chund_size || l_height < s_chund_size)
		{
			throw bc_invalid_operation_exception("Height map texture width and height must be greater than 64");
		}

		bc_height_map_texture_read_task l_height_map_copy_task(l_heightmap_texture->get_resource().get());
		l_render_system.add_render_task(l_height_map_copy_task);
		l_height_map_copy_task.wait();

		core::bc_unique_ptr<bcFLOAT> l_heights(reinterpret_cast<bcFLOAT*>(bcAlloc(sizeof(bcFLOAT) * l_width * l_height, core::bc_alloc_type::frame)));

		bcUBYTE* l_texture_data = l_height_map_copy_task.m_texure_buffer.get();
		bcFLOAT* l_dest = l_heights.get();
		auto* l_y_multiplier_value = p_context.m_parameter.get_value< bcINT >("y_multiplier");
		auto l_y_multiplier = bc_null_default(l_y_multiplier_value, 512);
		
		for (bcUINT32 l_i = 0, l_end = l_width * l_height; l_i < l_end; ++l_i)
		{
			bcFLOAT l_value = (l_texture_data[l_i * 4] / 255.f) * l_y_multiplier; // read red channel of texture

			l_dest[l_i] = l_value;
		}

		l_height_map_copy_task.m_texure_buffer.reset();

		physics::bc_mesh_buffer l_px_buffer = l_physics_system.create_height_field(l_width, l_height, l_heights.get());
		
		p_context.m_data = core::bc_vector_frame<bcBYTE>(l_px_buffer.get_buffer_size());
		std::memcpy(p_context.m_data.data(), l_px_buffer.get_buffer_pointer(), l_px_buffer.get_buffer_size());
	}

	void bc_height_map_loader_dx11::content_processing(core::bc_content_loader_context& p_context)
	{
		auto* l_content_loader = core::bc_get_service< core::bc_content_manager >();
		auto* l_game_system = core::bc_get_service< game::bc_game_system >();
		auto& l_render_system = l_game_system->get_render_system();
		auto& l_physics_system = l_game_system->get_physics_system();
		auto* l_device = &l_render_system.get_device();
		auto* l_physics = &l_physics_system.get_physics();

		auto l_heightmap_texture = l_content_loader->load< graphic::bc_texture2d_content >
		(
			p_context.get_allocator_alloc_type(),
			p_context.m_file_path.c_str(),
			core::bc_content_loader_parameter(p_context.m_parameter)
		);

		auto l_width = l_heightmap_texture->get_resource()->get_width();
		auto l_height = l_heightmap_texture->get_resource()->get_height();
		
		auto* l_x_pos_value = p_context.m_parameter.get_value< bcINT >("x_pos");
		auto* l_y_pos_value = p_context.m_parameter.get_value< bcINT >("y_pos");
		auto* l_z_pos_value = p_context.m_parameter.get_value< bcINT >("z_pos");
		auto* l_xz_multiplier_value = p_context.m_parameter.get_value< bcINT >("xz_multiplier");
		auto* l_y_multiplier_value = p_context.m_parameter.get_value< bcINT >("y_multiplier");
		auto* l_distance_detail_value = p_context.m_parameter.get_value< bcINT >("distance_detail");
		auto* l_height_detail_value = p_context.m_parameter.get_value< bcINT >("height_detail");

		auto l_position = core::bc_vector3f
		(
			bc_null_default(l_x_pos_value, 0),
			bc_null_default(l_y_pos_value, 0),
			bc_null_default(l_z_pos_value, 0)
		);
		bcUINT16 l_xz_multiplier = bc_null_default(l_xz_multiplier_value, 1);
		bcUINT16 l_y_multiplier = bc_null_default(l_y_multiplier_value, 512);
		bcUINT16 l_distance_detail = bc_null_default(l_distance_detail_value, 100);
		bcUINT16 l_height_detail = bc_null_default(l_height_detail_value, 20);

		bcUINT16 l_width_chunk_count = l_width / s_chund_size;
		bcUINT16 l_height_chunk_count = l_height / s_chund_size;

		core::bc_vector_frame< game::bc_vertex_pos_tex > l_vertices((l_width_chunk_count + 1) * (l_height_chunk_count + 1));
		core::bc_vector_frame< bcUINT16 > l_indices(l_width_chunk_count * l_height_chunk_count * 4);
		core::bc_vector_frame< std::array< bcINT32, 3 > > l_chunk_infoes(l_width_chunk_count * l_height_chunk_count);

		bcUINT16 l_counter = 0;

		for (bcINT16 z = -l_height_chunk_count / 2; z <= l_height_chunk_count / 2; ++z)
		{
			for (bcINT16 x = -l_width_chunk_count / 2; x <= l_width_chunk_count / 2; ++x)
			{
				game::bc_vertex_pos_tex& l_vertex = l_vertices[l_counter++];

				l_vertex.m_position = core::bc_vector3f
				(
					x * s_chund_size * l_xz_multiplier + l_position.x,
					0,
					z * s_chund_size * l_xz_multiplier + l_position.z
				);
				l_vertex.m_texcoord = core::bc_vector2f
				(
					(static_cast< bcFLOAT >(x) + (l_width_chunk_count / 2)) / l_width_chunk_count,
					(static_cast< bcFLOAT >(z) + (l_height_chunk_count / 2)) / l_height_chunk_count
				);
			}
		}

		l_counter = 0;

		for (bcUINT16 z = 0; z < l_height_chunk_count; z++)
		{
			for (bcUINT16 x = 0; x < l_width_chunk_count; x++)
			{
				l_indices[l_counter++] = x + z * (l_width_chunk_count + 1);
				l_indices[l_counter++] = x + (z + 1) * (l_width_chunk_count + 1);
				l_indices[l_counter++] = (x + 1) + (z + 1) * (l_width_chunk_count + 1);
				l_indices[l_counter++] = (x + 1) + z * (l_width_chunk_count + 1);
				/*
				1-------2
				|		|
				|		|
				0-------3
				*/
			}
		}

		for (bcUINT16 x = 0, end = l_width_chunk_count * l_height_chunk_count; x < end; ++x)
		{
			l_chunk_infoes[x] = std::array< bcINT32, 3 >{std::numeric_limits< bcINT32 >::max(), 0, 0};
		}

		_bc_parameter_buffer l_parameter;
		l_parameter.m_width = l_width;
		l_parameter.m_height = l_height;
		l_parameter.m_chunk_size = s_chund_size;
		l_parameter.m_xz_multiplier = l_xz_multiplier;
		l_parameter.m_y_multiplier = l_y_multiplier;
		l_parameter.m_distance_detail = l_distance_detail;
		l_parameter.m_height_detail = l_height_detail;

		auto l_resource_configure = graphic::bc_graphic_resource_configure();

		auto l_vbuffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				l_vertices.size(),
				sizeof(game::bc_vertex_pos_tex),
				graphic::bc_resource_usage::gpu_r
			)
			.as_vertex_buffer();
		auto l_ibuffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				l_indices.size(),
				sizeof(bcUINT16),
				graphic::bc_resource_usage::gpu_r
			)
			.as_index_buffer();
		auto l_chunk_info_buffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				l_width_chunk_count * l_height_chunk_count,
				sizeof(core::bc_vector3f),
				graphic::bc_resource_usage::gpu_rw,
				core::bc_enum::or({graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::unordered})
			)
			.as_structured_buffer(sizeof(core::bc_vector3f));
		auto l_parameter_cbuffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				1,
				sizeof(_bc_parameter_buffer),
				graphic::bc_resource_usage::gpu_rw
			)
			.as_constant_buffer();

		graphic::bc_subresource_data l_vbuffer_data(l_vertices.data(), 0, 0);
		graphic::bc_subresource_data l_ibuffer_data(l_indices.data(), 0, 0);
		graphic::bc_subresource_data l_chunk_info_buffer_data(l_chunk_infoes.data(), 0, 0);
		graphic::bc_subresource_data l_parameter_cbuffer_data(&l_parameter, 0, 0);

		//auto l_device_alloc_type = l_device->set_allocator_alloc_type(p_context.get_allocator_alloc_type());

		auto l_vertex_buffer = l_device->create_buffer(l_vbuffer_config, &l_vbuffer_data);
		auto l_index_buffer = l_device->create_buffer(l_ibuffer_config, &l_ibuffer_data);
		auto l_chunk_info_buffer = l_device->create_buffer(l_chunk_info_buffer_config, &l_chunk_info_buffer_data);
		auto l_parameter_cbuffer = l_device->create_buffer(l_parameter_cbuffer_config, &l_parameter_cbuffer_data);

		auto l_heightmap_view_config = l_resource_configure
			.as_resource_view()
			.as_texture_view(graphic::bc_format::R8G8B8A8_UNORM)
			.as_tex2d_shader_view(0, 1)
			.on_texture2d();
		auto l_chunk_info_view_config = l_resource_configure
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_shader_view(0, l_width_chunk_count * l_height_chunk_count)
			.as_structured_buffer();
		auto l_chunk_info_unordered_view_config = l_resource_configure
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_unordered_view(0, l_width_chunk_count * l_height_chunk_count)
			.as_structured_buffer();

		auto l_heightmap_view = l_device->create_resource_view(l_heightmap_texture->get_resource().get(), l_heightmap_view_config);
		auto l_chunk_info_view = l_device->create_resource_view(l_chunk_info_buffer.get(), l_chunk_info_view_config);
		auto l_chunk_info_unordered_view = l_device->create_resource_view(l_chunk_info_buffer.get(), l_chunk_info_unordered_view_config);

		//l_device->set_allocator_alloc_type(l_device_alloc_type);

		auto l_render_state = l_render_system.create_render_state
		(
			graphic::bc_primitive::control_point_4_patchlist,
			l_vertex_buffer.get(),
			sizeof(game::bc_vertex_pos_tex),
			0,
			l_index_buffer.get(),
			game::i16bit,
			l_indices.size(),
			0,
			{
				graphic::bc_resource_view_parameter(0, core::bc_enum::or({graphic::bc_shader_type::vertex, graphic::bc_shader_type::hull, graphic::bc_shader_type::domain, graphic::bc_shader_type::pixel}), l_heightmap_view),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::hull, l_chunk_info_view)
			},
			{
				graphic::bc_constant_buffer_parameter(0, core::bc_enum:: or ({ graphic::bc_shader_type::vertex, graphic::bc_shader_type::hull, graphic::bc_shader_type::domain, graphic::bc_shader_type::pixel }), l_parameter_cbuffer)
			}
		);

		physics::bc_mesh_buffer l_px_buffer = l_physics->read_to_mesh_buffer(p_context.m_data.data(), p_context.m_data.size());
		physics::bc_height_field_ref l_px_height_map = l_physics_system.create_height_field(l_px_buffer);

		auto l_height_map = bc_height_map_dx11
		(
			l_position,
			l_xz_multiplier,
			l_y_multiplier,
			l_distance_detail,
			l_height_detail, 
			l_heightmap_texture,
			l_heightmap_view,
			l_parameter_cbuffer,
			l_render_state,
			l_chunk_info_buffer,
			l_chunk_info_view,
			l_chunk_info_unordered_view,
			std::move(l_px_height_map)
		);

		p_context.set_result(std::move(l_height_map));
	}
}