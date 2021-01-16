// [04/29/2016 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Container/bcVector.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/bcExport.h"
#include "Game/System/Input/bcCamera.h"

namespace black_cat
{
	namespace game
	{
		class bc_sub_mesh;
		class bc_sub_mesh_transform;
		
		class BC_GAME_DLL bc_shape_generator_buffer
		{
		public:
			bc_shape_generator_buffer(core::bc_vector_movable< core::bc_vector3f >& p_vertices, core::bc_vector_movable< bcUINT32 >& p_indices);

			bcSIZE vertices_count() const noexcept;

			bcSIZE indices_size() const noexcept;

			void add_vertex(const core::bc_vector3f& p_vertex);

			void add_index(bcUINT32 p_index);

		private:
			core::bc_vector_movable< core::bc_vector3f >& m_vertices;
			core::bc_vector_movable< bcUINT32 >& m_indices;
		};

		class BC_GAME_DLL bc_shape_generator
		{
		public:
			static void create_wired_box(bc_shape_generator_buffer& p_buffer, const physics::bc_bound_box& p_box);

			static void create_wired_frustum(bc_shape_generator_buffer& p_buffer, const bci_camera::extend& p_camera_extend);
			
			static void create_wired_skeleton(bc_shape_generator_buffer& p_buffer, 
				const bc_sub_mesh& p_mesh, 
				const core::bc_matrix4f& p_world, 
				const bc_sub_mesh_transform& p_mesh_transforms);
		};
	}
}