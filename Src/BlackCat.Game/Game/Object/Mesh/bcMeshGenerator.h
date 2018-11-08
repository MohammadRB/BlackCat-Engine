// [04/29/2016 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Math/bcVector3f.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_mesh_generator
		{
		public:
			static void create_wired_box(core::bc_vector_frame<core::bc_vector3f>& p_vertices, core::bc_vector_frame<bcUINT32>& p_indices);

		protected:

		private:
			static void create_vertex_index_buffer(graphic::bc_device& p_device,
				core::bc_vector_frame<core::bc_vector3f>& p_vertices,
				core::bc_vector_frame<bcUINT32>& p_indices,
				graphic::bc_buffer_ptr& p_vertex_buffer,
				graphic::bc_buffer_ptr& p_index_buffer);
		};
	}
}