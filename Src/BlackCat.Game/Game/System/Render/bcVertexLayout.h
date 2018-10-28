// [05/06/2016 MRB]

#pragma once

#include "Core/Math/bcVector2f.h"
#include "Core/Math/bcVector3f.h"

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
			pos_tex = 0,				// Position_Texcoord
			pos_tex_nor = 1,			// Position_Texcoord_Normal
			pos_tex_nor_tan = 2		// Position_Texcoord_Normal_Tangent
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
	}
}