// [04/29/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcShapeGenerator.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"

namespace black_cat
{
	namespace game
	{
		bc_shape_generator_buffer::bc_shape_generator_buffer(core::bc_vector_movable< core::bc_vector3f >& p_vertices, core::bc_vector_movable< bcUINT32 >& p_indices)
			: m_vertices(p_vertices),
			m_indices(p_indices)
		{
		}

		bcSIZE bc_shape_generator_buffer::vertices_count() const noexcept
		{
			return m_vertices.size();
		}

		bcSIZE bc_shape_generator_buffer::indices_size() const noexcept
		{
			return m_indices.size();
		}

		void bc_shape_generator_buffer::add_vertex(const core::bc_vector3f& p_vertex)
		{
			m_vertices.push_back(p_vertex);
		}

		void bc_shape_generator_buffer::add_index(bcUINT32 p_index)
		{

			m_indices.push_back(p_index);
		}

		void bc_shape_generator::create_wired_box(bc_shape_generator_buffer& p_buffer, const physics::bc_bound_box& p_box)
		{
			const auto l_start_index = p_buffer.vertices_count();
			core::bc_array< core::bc_vector3f, 8> l_points;
			p_box.get_points(l_points);

			std::for_each(std::begin(l_points), std::end(l_points), [&](const core::bc_vector3f& p_point)
			{
				p_buffer.add_vertex(p_point);
			});

			p_buffer.add_index(l_start_index + 0);
			p_buffer.add_index(l_start_index + 1);
			p_buffer.add_index(l_start_index + 1);
			p_buffer.add_index(l_start_index + 2);
			p_buffer.add_index(l_start_index + 2);
			p_buffer.add_index(l_start_index + 3);
			p_buffer.add_index(l_start_index + 3);
			p_buffer.add_index(l_start_index + 0);

			p_buffer.add_index(l_start_index + 4);
			p_buffer.add_index(l_start_index + 5);
			p_buffer.add_index(l_start_index + 5);
			p_buffer.add_index(l_start_index + 6);
			p_buffer.add_index(l_start_index + 6);
			p_buffer.add_index(l_start_index + 7);
			p_buffer.add_index(l_start_index + 7);
			p_buffer.add_index(l_start_index + 4);

			p_buffer.add_index(l_start_index + 0);
			p_buffer.add_index(l_start_index + 4);
			p_buffer.add_index(l_start_index + 1);
			p_buffer.add_index(l_start_index + 5);
			p_buffer.add_index(l_start_index + 2);
			p_buffer.add_index(l_start_index + 6);
			p_buffer.add_index(l_start_index + 3);
			p_buffer.add_index(l_start_index + 7);
		}

		void bc_shape_generator::create_wired_frustum(bc_shape_generator_buffer& p_buffer, const bc_icamera::extend& p_camera_extend)
		{
			const auto l_start_index = p_buffer.vertices_count();
			
			p_buffer.add_vertex(p_camera_extend[0]);
			p_buffer.add_vertex(p_camera_extend[1]);
			p_buffer.add_vertex(p_camera_extend[2]);
			p_buffer.add_vertex(p_camera_extend[3]);
			p_buffer.add_vertex(p_camera_extend[4]);
			p_buffer.add_vertex(p_camera_extend[5]);
			p_buffer.add_vertex(p_camera_extend[6]);
			p_buffer.add_vertex(p_camera_extend[7]);

			p_buffer.add_index(l_start_index + 0);
			p_buffer.add_index(l_start_index + 1);
			p_buffer.add_index(l_start_index + 1);
			p_buffer.add_index(l_start_index + 2);
			p_buffer.add_index(l_start_index + 2);
			p_buffer.add_index(l_start_index + 3);
			p_buffer.add_index(l_start_index + 3);
			p_buffer.add_index(l_start_index + 0);

			p_buffer.add_index(l_start_index + 4);
			p_buffer.add_index(l_start_index + 5);
			p_buffer.add_index(l_start_index + 5);
			p_buffer.add_index(l_start_index + 6);
			p_buffer.add_index(l_start_index + 6);
			p_buffer.add_index(l_start_index + 7);
			p_buffer.add_index(l_start_index + 7);
			p_buffer.add_index(l_start_index + 4);

			p_buffer.add_index(l_start_index + 0);
			p_buffer.add_index(l_start_index + 4);
			p_buffer.add_index(l_start_index + 1);
			p_buffer.add_index(l_start_index + 5);
			p_buffer.add_index(l_start_index + 2);
			p_buffer.add_index(l_start_index + 6);
			p_buffer.add_index(l_start_index + 3);
			p_buffer.add_index(l_start_index + 7);
		}
	}
}