// [05/06/2016 MRB]

#pragma once

#include "Core/Math/bcVector2f.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcVector4f.h"

namespace black_cat
{
	namespace game
	{
		// enum index is a multiplier that indicate how many byte is needed to store this index
		enum bc_index_type
		{
			i16bit = sizeof(bcUINT16),
			i32bit = sizeof(bcUINT32)
		};

		enum class bc_vertex_type
		{
			none = 0,
			pos = 1,
			pos_tex = 2,
			pos_tex_nor = 3,
			pos_tex_nor_tan = 4,
			pos_tex_nor_tan_bon = 5
		};

		class bc_vertex_pos_tex
		{
		public:
			core::bc_vector3f m_position;
			core::bc_vector2f m_texcoord;
		};

		class bc_vertex_pos_tex_nor
		{
		public:
			core::bc_vector3f m_position;
			core::bc_vector2f m_texcoord;
			core::bc_vector3f m_normal;
		};

		class bc_vertex_pos_tex_nor_tan
		{
		public:
			core::bc_vector3f m_position;
			core::bc_vector2f m_texcoord;
			core::bc_vector3f m_normal;
			core::bc_vector3f m_tangent;
		};

		class bc_vertex_pos_tex_nor_tan_bon
		{
		public:
			core::bc_vector3f m_position;
			core::bc_vector2f m_texcoord;
			core::bc_vector3f m_normal;
			core::bc_vector3f m_tangent;
			core::bc_vector4i m_bone_indices;
			core::bc_vector4f m_bone_weights;
		};
	}
}