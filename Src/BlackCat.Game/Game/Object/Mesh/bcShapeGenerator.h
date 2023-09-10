// [29/04/2016 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Container/bcVector.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "PhysicsImp/Shape/bcShape.h"
#include "Game/bcExport.h"
#include "Game/System/Input/bcCamera.h"

namespace black_cat::game
{
	class bc_sub_mesh;
	class bc_sub_mesh_transform_mat4;
		
	class BC_GAME_DLL bc_shape_generator_buffer
	{
	public:
		bc_shape_generator_buffer(core::bc_vector_movable<core::bc_vector3f>& p_vertices, core::bc_vector_movable<bcUINT32>& p_indices);

		bcSIZE vertices_count() const noexcept;

		bcSIZE indices_size() const noexcept;

		void add_vertex(const core::bc_vector3f& p_vertex);

		void add_index(bcUINT32 p_index);

	private:
		core::bc_vector_movable<core::bc_vector3f>& m_vertices;
		core::bc_vector_movable<bcUINT32>& m_indices;
	};

	class BC_GAME_DLL bc_shape_generator
	{
	public:
		static void create_wired_bound_box(bc_shape_generator_buffer& p_buffer, const physics::bc_bound_box& p_box);
			
		static void create_wired_box(bc_shape_generator_buffer& p_buffer, const physics::bc_shape_box& p_box, const physics::bc_transform& p_transform);

		static void create_wired_sphere(bc_shape_generator_buffer& p_buffer, const physics::bc_shape_sphere& p_sphere, const physics::bc_transform& p_transform);
			
		static void create_wired_capsule(bc_shape_generator_buffer& p_buffer, const physics::bc_shape_capsule& p_capsule, const physics::bc_transform& p_transform);
			
		static void create_wired_frustum(bc_shape_generator_buffer& p_buffer, const bci_camera::extend_points& p_camera_extend);
			
		static void create_wired_skeleton(bc_shape_generator_buffer& p_buffer, 
		                                  const bc_sub_mesh& p_mesh, 
		                                  const core::bc_matrix4f& p_world, 
		                                  const bc_sub_mesh_mat4_transform& p_mesh_transforms);
	};
}
