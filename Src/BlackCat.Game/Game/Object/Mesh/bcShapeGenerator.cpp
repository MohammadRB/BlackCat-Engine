// [29/04/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Math/bcCoordinate.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/Object/Mesh/bcSubMeshTransform.h"
#include "Game/Object/Mesh/bcShapeGenerator.h"

namespace black_cat
{
	namespace game
	{
		bc_shape_generator_buffer::bc_shape_generator_buffer(core::bc_vector_movable<core::bc_vector3f>& p_vertices, core::bc_vector_movable<bcUINT32>& p_indices)
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

		void bc_shape_generator::create_wired_bound_box(bc_shape_generator_buffer& p_buffer, const physics::bc_bound_box& p_box)
		{
			const auto l_start_index = p_buffer.vertices_count();
			core::bc_array<core::bc_vector3f, 8> l_points;
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

		void bc_shape_generator::create_wired_box(bc_shape_generator_buffer& p_buffer, const physics::bc_shape_box& p_box, const physics::bc_transform& p_transform)
		{
			const auto l_start_index = p_buffer.vertices_count();
			core::bc_array<core::bc_vector3f, 8> l_points;
			p_box.get_points(l_points);

			std::for_each(std::begin(l_points), std::end(l_points), [&](const core::bc_vector3f& p_point)
			{
				p_buffer.add_vertex(p_transform.transform(p_point));
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

		void bc_shape_generator::create_wired_sphere(bc_shape_generator_buffer& p_buffer, const physics::bc_shape_sphere& p_sphere, const physics::bc_transform& p_transform)
		{
			const auto l_start_index = p_buffer.vertices_count();
			const auto l_circle_count = 4U;
			const auto l_circle_vertex_count = 32U;

			for(auto l_circle_ite = 0U; l_circle_ite < l_circle_count; ++l_circle_ite)
			{
				const auto l_circle_angle = (180.0f / l_circle_count) * l_circle_ite;
				core::bc_matrix3f l_circle_rotation;

				if (graphic::bc_render_api_info::use_left_handed())
				{
					l_circle_rotation.rotation_y_lh(core::bc_to_radian(l_circle_angle));
				}
				else
				{
					l_circle_rotation.rotation_y_rh(core::bc_to_radian(l_circle_angle));
				}
				
				for (auto l_vertex_ite = 0U; l_vertex_ite < l_circle_vertex_count; ++l_vertex_ite)
				{
					const auto l_vertex_angle = (360.0f / l_circle_vertex_count) * l_vertex_ite;
					core::bc_matrix3f l_vertex_rotation;
					
					if(graphic::bc_render_api_info::use_left_handed())
					{
						l_vertex_rotation.rotation_z_lh(core::bc_to_radian(l_vertex_angle));
					}
					else
					{
						l_vertex_rotation.rotation_z_rh(core::bc_to_radian(l_vertex_angle));
					}

					auto l_vertex = l_vertex_rotation * core::bc_vector3f::up();
					l_vertex = l_circle_rotation * l_vertex;
					l_vertex *= p_sphere.get_radius();
					l_vertex = p_transform.transform(l_vertex);

					p_buffer.add_vertex(l_vertex);
				}

				for (auto l_vertex_ite = 0U; l_vertex_ite < l_circle_vertex_count; ++l_vertex_ite)
				{
					p_buffer.add_index(l_start_index + (l_circle_ite * l_circle_vertex_count) + l_vertex_ite);
					p_buffer.add_index(l_start_index + (l_circle_ite * l_circle_vertex_count) + ((l_vertex_ite + 1) % l_circle_vertex_count));
				}
			}
		}

		void bc_shape_generator::create_wired_capsule(bc_shape_generator_buffer& p_buffer, const physics::bc_shape_capsule& p_capsule, const physics::bc_transform& p_transform)
		{
			const auto l_sphere1_center = core::bc_vector3f::right() * p_capsule.get_half_height();
			const auto l_sphere2_center = core::bc_vector3f::right() * core::bc_vector3f(-1, 0, 0) * p_capsule.get_half_height();
			
			create_wired_sphere
			(
				p_buffer, 
				physics::bc_shape_sphere(p_capsule.get_radius()), 
				p_transform.transform(physics::bc_transform(l_sphere1_center))
			);
			create_wired_sphere
			(
				p_buffer,
				physics::bc_shape_sphere(p_capsule.get_radius()),
				p_transform.transform(physics::bc_transform(l_sphere2_center))
			);

			const auto l_start_index = p_buffer.vertices_count();
			const auto l_body_segment_count = 8;
			const auto l_body_segment_point1 = l_sphere1_center + core::bc_vector3f::up() * p_capsule.get_radius();
			const auto l_body_segment_point2 = l_sphere2_center + core::bc_vector3f::up() * p_capsule.get_radius();
						
			for(auto l_segment_ite = 0U; l_segment_ite < l_body_segment_count; ++l_segment_ite)
			{
				auto l_segment_angle = (360.0f / l_body_segment_count) * l_segment_ite;
				core::bc_matrix3f l_segment_rotation;

				if (graphic::bc_render_api_info::use_left_handed())
				{
					l_segment_rotation.rotation_x_lh(core::bc_to_radian(l_segment_angle));
				}
				else
				{
					l_segment_rotation.rotation_x_rh(core::bc_to_radian(l_segment_angle));
				}
				
				auto l_segment_point1 = l_segment_rotation * l_body_segment_point1;
				auto l_segment_point2 = l_segment_rotation * l_body_segment_point2;

				l_segment_point1 = p_transform.transform(l_segment_point1);
				l_segment_point2 = p_transform.transform(l_segment_point2);
				
				p_buffer.add_vertex(l_segment_point1);
				p_buffer.add_vertex(l_segment_point2);
				p_buffer.add_index(l_start_index + (l_segment_ite * 2));
				p_buffer.add_index(l_start_index + (l_segment_ite * 2) + 1);
			}
		}

		void bc_shape_generator::create_wired_frustum(bc_shape_generator_buffer& p_buffer, const bci_camera::extend& p_camera_extend)
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

		void bc_shape_generator::create_wired_skeleton(bc_shape_generator_buffer& p_buffer, 
			const bc_sub_mesh& p_mesh, 
			const core::bc_matrix4f& p_world, 
			const bc_sub_mesh_mat4_transform& p_mesh_transforms)
		{
			auto l_root_transform = p_world;
			p_mesh.iterate_over_nodes(l_root_transform, [&p_buffer, &p_world, &p_mesh_transforms](const bc_mesh_node& p_node, core::bc_matrix4f& p_parent_transform)
			{
				const auto l_start_index = p_buffer.vertices_count();
				const auto l_node_transform = p_mesh_transforms.get_node_transform(p_node) * p_world;
				
				p_buffer.add_vertex(p_parent_transform.get_translation());
				p_buffer.add_vertex(l_node_transform.get_translation());
				p_buffer.add_index(l_start_index + 0);
				p_buffer.add_index(l_start_index + 1);

				return l_node_transform;
			},
			[](const bc_mesh_node&, const core::bc_matrix4f&)
			{
			});
		}
	}
}