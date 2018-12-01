// [01/18/2017 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcArray.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/System/Physics/bcPhysicsShapeUtility.h"

namespace black_cat
{
	namespace game
	{
		physics::bc_shape_sphere bc_extract_sphere_from_points(physics::bc_bounded_strided_typed_data<core::bc_vector3f> p_vertices)
		{
			bcFLOAT l_min_x = std::numeric_limits<bcFLOAT>::max();
			bcFLOAT l_max_x = std::numeric_limits<bcFLOAT>::min();

			for (bcUINT32 l_index = 0; l_index < p_vertices.m_count; ++l_index)
			{
				auto& l_vertex = p_vertices.at(l_index);

				l_min_x = std::min(l_min_x, l_vertex.x);
				l_max_x = std::max(l_max_x, l_vertex.x);
			}

			bcFLOAT l_radius = (l_max_x - l_min_x) / 2;
			return physics::bc_shape_sphere(l_radius);
		}

		physics::bc_shape_box bc_extract_box_from_points(physics::bc_bounded_strided_typed_data< core::bc_vector3f > p_vertices)
		{
			bcFLOAT l_min_x, l_min_y, l_min_z;
			bcFLOAT l_max_x, l_max_y, l_max_z;
			l_min_x = l_min_y = l_min_z = std::numeric_limits< bcFLOAT >::max();
			l_max_x = l_max_y = l_max_z = std::numeric_limits< bcFLOAT >::min();

			for (bcUINT32 l_index = 0; l_index < p_vertices.m_count; ++l_index)
			{
				auto& l_vertex = p_vertices.at(l_index);

				l_min_x = std::min(l_min_x, l_vertex.x);
				l_min_y = std::min(l_min_y, l_vertex.y);
				l_min_z = std::min(l_min_z, l_vertex.z);
				l_max_x = std::max(l_max_x, l_vertex.x);
				l_max_y = std::max(l_max_y, l_vertex.y);
				l_max_z = std::max(l_max_z, l_vertex.z);
			}

			return physics::bc_shape_box((l_max_x - l_min_x) / 2, (l_max_y - l_min_y) / 2, (l_max_z - l_min_z) / 2);
		}

		physics::bc_shape_capsule bc_extract_capsule_from_points(physics::bc_bounded_strided_typed_data<core::bc_vector3f> p_vertices)
		{
			bcFLOAT l_min_x, l_min_y;
			bcFLOAT l_max_x, l_max_y;
			l_min_x = l_min_y = std::numeric_limits< bcFLOAT >::max();
			l_max_x = l_max_y = std::numeric_limits< bcFLOAT >::min();

			for (bcUINT32 l_index = 0; l_index < p_vertices.m_count; ++l_index)
			{
				auto& l_vertex = p_vertices.at(l_index);

				l_min_x = std::min(l_min_x, l_vertex.x);
				l_min_y = std::min(l_min_y, l_vertex.y);
				l_max_x = std::max(l_max_x, l_vertex.x);
				l_max_y = std::max(l_max_y, l_vertex.y);
			}

			bcFLOAT l_radius = (l_max_y - l_min_y) / 2;
			bcFLOAT l_half_height = ((l_max_x - l_min_x) - l_radius * 2) / 2;

			return physics::bc_shape_capsule(l_half_height, l_radius);
		}

		physics::bc_convex_mesh_desc bc_extract_convex_from_points(physics::bc_bounded_strided_typed_data<core::bc_vector3f> p_vertices)
		{
			return physics::bc_convex_mesh_desc(p_vertices);
		}

		physics::bc_triangle_mesh_desc bc_extract_mesh_from_points(physics::bc_bounded_strided_typed_data<core::bc_vector3f> p_vertices, physics::bc_bounded_strided_data p_indices)
		{
			return physics::bc_triangle_mesh_desc
			(
				p_vertices,
				p_indices,
				p_indices.m_stride == sizeof(bcUINT16) ? physics::bc_triangle_mesh_flag::use_16bit_index : core::bc_enum::none<physics::bc_triangle_mesh_flag>()
			);
		}

		physics::bc_bound_box bc_convert_shape_box_to_bound_box(const physics::bc_shape_box& p_shape, const physics::bc_transform& p_shape_transform)
		{
			return bc_convert_shape_box_to_bound_box(p_shape, p_shape_transform.get_matrix4());
		}

		physics::bc_bound_box bc_convert_shape_box_to_bound_box(const physics::bc_shape_box& p_shape, const core::bc_matrix4f& p_shape_transform)
		{
			const auto l_z_sign = graphic::bc_render_api_info::is_left_handed() ? +1 : -1;
			const core::bc_vector3f l_box_extends = p_shape.get_half_extends();

			core::bc_array< core::bc_vector4f, 8> l_points
			{
				core::bc_vector4f(-l_box_extends.x, l_box_extends.y, l_box_extends.z * l_z_sign, 0),
				core::bc_vector4f(-l_box_extends.x, l_box_extends.y, -l_box_extends.z * l_z_sign, 0),
				core::bc_vector4f(l_box_extends.x, l_box_extends.y, l_box_extends.z * l_z_sign, 0),
				core::bc_vector4f(l_box_extends.x, l_box_extends.y, -l_box_extends.z * l_z_sign, 0),
				core::bc_vector4f(-l_box_extends.x, -l_box_extends.y, l_box_extends.z * l_z_sign, 0),
				core::bc_vector4f(-l_box_extends.x, -l_box_extends.y, -l_box_extends.z * l_z_sign, 0),
				core::bc_vector4f(l_box_extends.x, -l_box_extends.y, l_box_extends.z * l_z_sign, 0),
				core::bc_vector4f(l_box_extends.x, -l_box_extends.y, -l_box_extends.z * l_z_sign, 0)
			};
			std::transform(std::begin(l_points), std::end(l_points), std::begin(l_points), [&](const core::bc_vector4f& p_point)
			{
				return p_shape_transform * p_point;
			});

			bcFLOAT l_min_x, l_min_y, l_min_z;
			bcFLOAT l_max_x, l_max_y, l_max_z;
			l_min_x = l_min_y = l_min_z = std::numeric_limits< bcFLOAT >::max();
			l_max_x = l_max_y = l_max_z = std::numeric_limits< bcFLOAT >::min();

			for (bcUINT32 l_index = 0; l_index < l_points.size(); ++l_index)
			{
				auto& l_vertex = l_points.at(l_index);

				l_min_x = std::min(l_min_x, l_vertex.x);
				l_min_y = std::min(l_min_y, l_vertex.y);
				l_min_z = std::min(l_min_z, l_vertex.z);
				l_max_x = std::max(l_max_x, l_vertex.x);
				l_max_y = std::max(l_max_y, l_vertex.y);
				l_max_z = std::max(l_max_z, l_vertex.z);
			}

			return physics::bc_bound_box
			(
				p_shape_transform.get_translation(),
				core::bc_vector3f((l_max_x - l_min_x) / 2, (l_max_y - l_min_y) / 2, (l_max_z - l_min_z) / 2)
			);
			/*auto l_result = physics::bc_bound_box
			(
				p_shape_transform.get_translation(), 
				core::bc_vector3f((l_max_x - l_min_x) / 2, (l_max_y - l_min_y) / 2, (l_max_z - l_min_z) / 2)
			);*/
			//l_result.transform(p_shape_transform.get_rotation());
			//return l_result;
		}
	}
}