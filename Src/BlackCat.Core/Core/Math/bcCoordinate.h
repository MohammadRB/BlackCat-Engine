// [29/01/2017 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/Math/bcMatrix4f.h"

namespace black_cat::core
{
	constexpr bcFLOAT g_pi = 3.14159265358979323846f;

	inline bcFLOAT bc_to_radian(bcFLOAT p_degree)
	{
		return (p_degree * g_pi) / 180;
	}

	inline bcFLOAT bc_to_degree(bcFLOAT p_radian)
	{
		return (p_radian * 180) / g_pi;
	}

	inline void bc_to_right_hand(bc_vector3f& p_vector)
	{
		p_vector.z = -p_vector.z;
	}

	inline void bc_to_left_hand(bc_vector3f& p_vector)
	{
		p_vector.z = -p_vector.z;
	}

	inline void bc_to_right_hand(bc_matrix3f& p_mat)
	{
		p_mat[2] = -p_mat[2];
		p_mat[5] = -p_mat[5];
		p_mat[6] = -p_mat[6];
		p_mat[7] = -p_mat[7];
	}

	inline void bc_to_left_hand(bc_matrix3f& p_mat)
	{
		p_mat[2] = -p_mat[2];
		p_mat[5] = -p_mat[5];
		p_mat[6] = -p_mat[6];
		p_mat[7] = -p_mat[7];
	}

	inline void bc_to_right_hand(bc_matrix4f& p_mat)
	{
		p_mat[2] = -p_mat[2];
		p_mat[6] = -p_mat[6];
		p_mat[8] = -p_mat[8];
		p_mat[9] = -p_mat[9];
		p_mat[14] = -p_mat[14];
	}

	inline void bc_to_left_hand(bc_matrix4f& p_mat)
	{
		p_mat[2] = -p_mat[2];
		p_mat[6] = -p_mat[6];
		p_mat[8] = -p_mat[8];
		p_mat[9] = -p_mat[9];
		p_mat[14] = -p_mat[14];
	}
}
