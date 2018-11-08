// [04/29/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcMeshGenerator.h"
#include "GraphicImp/Resource/bcResourceConfig.h"

namespace black_cat
{
	namespace game
	{
		void bc_mesh_generator::create_wired_box(core::bc_vector_frame<core::bc_vector3f>& p_vertices, core::bc_vector_frame<bcUINT32>& p_indices)
		{
			p_vertices.reserve(8);
			p_indices.reserve(12);

			p_vertices[0] = core::bc_vector3f(-1, 1, 1);
			p_vertices[1] = core::bc_vector3f(1, 1, 1);
			p_vertices[2] = core::bc_vector3f(1, 1, -1);
			p_vertices[3] = core::bc_vector3f(-1, 1, -1);
			p_vertices[4] = core::bc_vector3f(-1, -1, 1);
			p_vertices[5] = core::bc_vector3f(1, -1, 1);
			p_vertices[6] = core::bc_vector3f(1, -1, -1);
			p_vertices[7] = core::bc_vector3f(-1, -1, -1);

			p_indices[0] = 0;
			p_indices[0] = 1;
			p_indices[0] = 1;
			p_indices[0] = 2;
			p_indices[0] = 2;
			p_indices[0] = 3;
			p_indices[0] = 3;
			p_indices[0] = 0;
			p_indices[0] = 4;
			p_indices[0] = 5;
			p_indices[0] = 5;
			p_indices[0] = 6;
			p_indices[0] = 6;
			p_indices[0] = 7;
			p_indices[0] = 7;
			p_indices[0] = 4;
			p_indices[0] = 0;
			p_indices[0] = 4;
			p_indices[0] = 1;
			p_indices[0] = 5;
			p_indices[0] = 2;
			p_indices[0] = 6;
			p_indices[0] = 3;
			p_indices[0] = 7;
		}

		void bc_mesh_generator::create_vertex_index_buffer(graphic::bc_device& p_device,
			core::bc_vector_frame<core::bc_vector3f>& p_vertices,
			core::bc_vector_frame<bcUINT32>& p_indices,
			graphic::bc_buffer_ptr& p_vertex_buffer,
			graphic::bc_buffer_ptr& p_index_buffer)
		{
			graphic::bc_graphic_resource_configure l_resource_configure;
			graphic::bc_buffer_config l_vertex_buffer_config = l_resource_configure
				.as_resource()
				.as_buffer
				(
					p_vertices.size(),
					sizeof(core::bc_vector3f),
					graphic::bc_resource_usage::gpu_r,
					graphic::bc_resource_view_type::none,
					false
					)
				.as_vertex_buffer();
			graphic::bc_buffer_config l_index_buffer_config = l_resource_configure
				.as_resource()
				.as_buffer
				(
					p_indices.size(),
					sizeof(bcUINT16),
					graphic::bc_resource_usage::gpu_r,
					graphic::bc_resource_view_type::none,
					false
					)
				.as_index_buffer();

			graphic::bc_subresource_data l_vertex_data = graphic::bc_subresource_data(p_vertices.data(), 0, 0);
			graphic::bc_subresource_data l_index_data = graphic::bc_subresource_data(p_indices.data(), 0, 0);

			p_vertex_buffer = p_device.create_buffer(l_vertex_buffer_config, &l_vertex_data);
			p_index_buffer = p_device.create_buffer(l_index_buffer_config, &l_index_data);
		}
	}
}