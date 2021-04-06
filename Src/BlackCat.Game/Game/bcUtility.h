// [12/31/2020 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/bcUtility.h"
#include "GraphicImp/bcRenderApiInfo.h"

namespace black_cat
{
	inline void bc_matrix3f_rotation_between_two_vector(core::bc_matrix3f& p_matrix, const core::bc_vector3f& p_v1, const core::bc_vector3f& p_v2)
	{
		if (graphic::bc_render_api_info::use_left_handed())
		{
			p_matrix.rotation_between_two_vector_lh(p_v1, p_v2);
		}
		else
		{
			p_matrix.rotation_between_two_vector_rh(p_v1, p_v2);
		}
	}
}