// [04/29/2016 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Math/bcVector3f.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_shape_generator_buffer
		{
		public:
			bc_shape_generator_buffer(core::bc_vector_movale< core::bc_vector3f >& p_vertices, core::bc_vector_movale< bcUINT32 >& p_indices);

			bcSIZE vertices_count() const;

			bcSIZE indices_size() const;

			void add_vertex(const core::bc_vector3f& p_vertex);

			void add_index(bcUINT32 p_index);

		private:
			core::bc_vector_movale< core::bc_vector3f >& m_vertices;
			core::bc_vector_movale< bcUINT32 >& m_indices;
		};

		class BC_GAME_DLL bc_shape_generator
		{
		public:
			static void create_wired_box(bc_shape_generator_buffer& p_buffer, const physics::bc_bound_box& p_box);
		};
	}
}