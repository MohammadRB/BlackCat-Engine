// [01/18/2017 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcArray.h"
#include "Core/Container/bcVector.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/System/Physics/bcPhysicsShapeUtility.h"
#include "Game/Object/Mesh/bcHeightMap.h"

namespace black_cat
{
	namespace game
	{
		physics::bc_shape_sphere bc_extract_sphere_from_points(physics::bc_bounded_strided_typed_data<core::bc_vector3f> p_vertices)
		{
			bcFLOAT l_min_x = std::numeric_limits<bcFLOAT>::max();
			bcFLOAT l_max_x = std::numeric_limits<bcFLOAT>::lowest();

			for (bcUINT32 l_index = 0; l_index < p_vertices.m_count; ++l_index)
			{
				const auto& l_vertex = p_vertices.at(l_index);

				l_min_x = std::min(l_min_x, l_vertex.x);
				l_max_x = std::max(l_max_x, l_vertex.x);
			}

			const bcFLOAT l_radius = (l_max_x - l_min_x) / 2;
			return physics::bc_shape_sphere(l_radius);
		}

		physics::bc_shape_box bc_extract_box_from_points(physics::bc_bounded_strided_typed_data< core::bc_vector3f > p_vertices)
		{
			bcFLOAT l_min_x, l_min_y, l_min_z;
			bcFLOAT l_max_x, l_max_y, l_max_z;
			l_min_x = l_min_y = l_min_z = std::numeric_limits< bcFLOAT >::max();
			l_max_x = l_max_y = l_max_z = std::numeric_limits< bcFLOAT >::lowest();

			for (bcUINT32 l_index = 0; l_index < p_vertices.m_count; ++l_index)
			{
				const auto& l_vertex = p_vertices.at(l_index);

				l_min_x = std::min(l_min_x, l_vertex.x);
				l_min_y = std::min(l_min_y, l_vertex.y);
				l_min_z = std::min(l_min_z, l_vertex.z);
				l_max_x = std::max(l_max_x, l_vertex.x);
				l_max_y = std::max(l_max_y, l_vertex.y);
				l_max_z = std::max(l_max_z, l_vertex.z);
			}

			return physics::bc_shape_box((l_max_x - l_min_x) / 2, (l_max_y - l_min_y) / 2, (l_max_z - l_min_z) / 2);
		}

		physics::bc_bound_box bc_extract_bound_box_from_points(physics::bc_bounded_strided_typed_data< core::bc_vector3f > p_vertices)
		{
			bcFLOAT l_min_x, l_min_y, l_min_z;
			bcFLOAT l_max_x, l_max_y, l_max_z;
			l_min_x = l_min_y = l_min_z = std::numeric_limits< bcFLOAT >::max();
			l_max_x = l_max_y = l_max_z = std::numeric_limits< bcFLOAT >::lowest();

			for (bcUINT32 l_index = 0; l_index < p_vertices.m_count; ++l_index)
			{
				const auto& l_vertex = p_vertices.at(l_index);

				l_min_x = std::min(l_min_x, l_vertex.x);
				l_min_y = std::min(l_min_y, l_vertex.y);
				l_min_z = std::min(l_min_z, l_vertex.z);
				l_max_x = std::max(l_max_x, l_vertex.x);
				l_max_y = std::max(l_max_y, l_vertex.y);
				l_max_z = std::max(l_max_z, l_vertex.z);
			}

			return physics::bc_bound_box
			(
				core::bc_vector3f((l_max_x + l_min_x) / 2, (l_max_y + l_min_y) / 2, (l_max_z + l_min_z) / 2),
				core::bc_vector3f((l_max_x - l_min_x) / 2, (l_max_y - l_min_y) / 2, (l_max_z - l_min_z) / 2)
			);
		}

		physics::bc_shape_capsule bc_extract_capsule_from_points(physics::bc_bounded_strided_typed_data<core::bc_vector3f> p_vertices)
		{
			bcFLOAT l_min_x, l_min_y;
			bcFLOAT l_max_x, l_max_y;
			l_min_x = l_min_y = std::numeric_limits< bcFLOAT >::max();
			l_max_x = l_max_y = std::numeric_limits< bcFLOAT >::lowest();

			for (bcUINT32 l_index = 0; l_index < p_vertices.m_count; ++l_index)
			{
				const auto& l_vertex = p_vertices.at(l_index);

				l_min_x = std::min(l_min_x, l_vertex.x);
				l_min_y = std::min(l_min_y, l_vertex.y);
				l_max_x = std::max(l_max_x, l_vertex.x);
				l_max_y = std::max(l_max_y, l_vertex.y);
			}

			const bcFLOAT l_radius = (l_max_y - l_min_y) / 2;
			const bcFLOAT l_half_height = ((l_max_x - l_min_x) - l_radius * 2) / 2;

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
			core::bc_array< core::bc_vector3f, 8> l_points;
			p_shape.get_points(l_points);

			std::transform(std::begin(l_points), std::end(l_points), std::begin(l_points), [&](const core::bc_vector3f& p_point)
			{
				return (p_shape_transform * core::bc_vector4f(p_point, 1)).xyz();
			});

			bcFLOAT l_min_x, l_min_y, l_min_z;
			bcFLOAT l_max_x, l_max_y, l_max_z;
			l_min_x = l_min_y = l_min_z = std::numeric_limits< bcFLOAT >::max();
			l_max_x = l_max_y = l_max_z = std::numeric_limits< bcFLOAT >::lowest();

			for (auto& l_vertex : l_points)
			{
				l_min_x = std::min(l_min_x, l_vertex.x);
				l_min_y = std::min(l_min_y, l_vertex.y);
				l_min_z = std::min(l_min_z, l_vertex.z);
				l_max_x = std::max(l_max_x, l_vertex.x);
				l_max_y = std::max(l_max_y, l_vertex.y);
				l_max_z = std::max(l_max_z, l_vertex.z);
			}

			return physics::bc_bound_box
			(
				core::bc_vector3f((l_max_x + l_min_x) / 2, (l_max_y + l_min_y) / 2, (l_max_z + l_min_z) / 2),
				core::bc_vector3f((l_max_x - l_min_x) / 2, (l_max_y - l_min_y) / 2, (l_max_z - l_min_z) / 2)
			);
		}

		physics::bc_transform bc_convert_to_height_map_transform(const bc_height_map& p_height_map, const physics::bc_transform& p_transform)
		{
			const auto l_half_width = (p_height_map.get_width() * p_height_map.get_xz_multiplier()) / 2;
			const auto l_half_height = (p_height_map.get_height() * p_height_map.get_xz_multiplier()) / 2;
			const auto l_position = p_transform.get_position() + core::bc_vector3f(-l_half_width, 0, l_half_height);

			return {l_position, p_transform.get_matrix3()};
		}

		physics::bc_shape_ref bc_copy_shape(physics::bc_physics& p_physics, physics::bc_shape p_shape)
		{
			physics::bc_shape_ref l_shape;

			const auto l_material_count = p_shape.get_material_count();
			core::bc_vector_frame<physics::bc_material> l_materials(l_material_count);

			p_shape.get_materials(l_materials.data(), l_material_count);
			
			switch (p_shape.get_type())
			{
			case physics::bc_shape_type::sphere:
			{
				physics::bc_shape_sphere l_sphere(0);
				p_shape.as_sphere(l_sphere);

				l_shape = p_physics.create_shape(l_sphere, l_materials.data(), l_material_count);
				break;
			}
			case physics::bc_shape_type::plane:
			{
				physics::bc_shape_plane l_plane(core::bc_vector3f(), 0);
				p_shape.as_plane(l_plane);

				l_shape = p_physics.create_shape(l_plane, l_materials.data(), l_material_count);
				break;
			}
			case physics::bc_shape_type::capsule:
			{
				physics::bc_shape_capsule l_capsule(0, 0);
				p_shape.as_capsule(l_capsule);

				l_shape = p_physics.create_shape(l_capsule, l_materials.data(), l_material_count);
				break;
			}
			case physics::bc_shape_type::box:
			{
				physics::bc_shape_box l_box(0, 0, 0);
				p_shape.as_box(l_box);

				l_shape = p_physics.create_shape(l_box, l_materials.data(), l_material_count);
				break;
			}
			case physics::bc_shape_type::convex_mesh:
			{
				const physics::bc_convex_mesh l_convex_mesh;
				physics::bc_shape_convex_mesh _convex_shape(l_convex_mesh);
				p_shape.as_convex_mesh(_convex_shape);

				l_shape = p_physics.create_shape(_convex_shape, l_materials.data(), l_material_count);
				break;
			}
			case physics::bc_shape_type::triangle_mesh:
			{
				const physics::bc_triangle_mesh l_triangle_mesh;
				physics::bc_shape_triangle_mesh l_triangle_shape(l_triangle_mesh);
				p_shape.as_triangle_mesh(l_triangle_shape);

				l_shape = p_physics.create_shape(l_triangle_shape, l_materials.data(), l_material_count);
				break;
			}
			case physics::bc_shape_type::height_field:
			{
				const physics::bc_height_field l_height_field;
				physics::bc_shape_height_field l_l_height_field_shape(l_height_field, 0, 0);
				p_shape.as_height_field(l_l_height_field_shape);

				l_shape = p_physics.create_shape(l_l_height_field_shape, l_materials.data(), l_material_count);
				break;
			}
			default:
				BC_ASSERT(false);
			}

			l_shape->set_contact_offset(p_shape.get_contact_offset());
			l_shape->set_rest_offset(p_shape.get_rest_offset());
			l_shape->set_query_group(p_shape.get_query_group());
			l_shape->set_collision_group(p_shape.get_collision_group());
			l_shape->set_high_detail_query_shape(p_shape.get_high_detail_query_shape());
			l_shape->set_flags(p_shape.get_flags());
			l_shape->set_query_flags(p_shape.get_query_flags());
			l_shape->set_data(p_shape.get_data());

			return l_shape;
		}
	}
}