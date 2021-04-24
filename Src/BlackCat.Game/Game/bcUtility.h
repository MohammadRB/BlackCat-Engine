// [12/31/2020 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcVector4f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/bcUtility.h"
#include "GraphicImp/bcRenderApiInfo.h"

namespace black_cat
{
	inline core::bc_matrix3f bc_matrix3f_rotation_between_two_vector(const core::bc_vector3f& p_v1, const core::bc_vector3f& p_v2)
	{
		core::bc_matrix3f l_matrix;
		
		if (graphic::bc_render_api_info::use_left_handed())
		{
			l_matrix.rotation_between_two_vector_lh(p_v1, p_v2);
		}
		else
		{
			l_matrix.rotation_between_two_vector_rh(p_v1, p_v2);
		}

		return l_matrix;
	}

	inline core::bc_matrix3f bc_matrix3f_rotation_between_two_vector_checked(const core::bc_vector3f& p_v1, const core::bc_vector3f& p_v2)
	{
		core::bc_matrix3f l_matrix;

		if (graphic::bc_render_api_info::use_left_handed())
		{
			l_matrix.rotation_between_two_vector_checked_lh(p_v1, p_v2);
		}
		else
		{
			l_matrix.rotation_between_two_vector_checked_rh(p_v1, p_v2);
		}

		return l_matrix;
	}

	inline core::bc_matrix3f bc_matrix3f_rotation_euler(const core::bc_vector3f& p_axis, bcFLOAT p_radians)
	{
		core::bc_matrix3f l_matrix;

		if (graphic::bc_render_api_info::use_left_handed())
		{
			l_matrix.rotation_euler_lh(p_axis, p_radians);
		}
		else
		{
			l_matrix.rotation_euler_lh(p_axis, p_radians);
		}

		return l_matrix;
	}
	
	inline core::bc_vector4f bc_matrix4f_decompose_to_euler_rotation(const core::bc_matrix4f& p_transform)
	{
		const auto l_up = p_transform.get_row(1).xyz();
		const auto l_up_rotation = core::bc_to_degree(std::acos(core::bc_vector3f::dot(core::bc_vector3f::right(), p_transform.get_row(0).xyz())));

		return core::bc_vector4f(l_up, l_up_rotation);
	}
}