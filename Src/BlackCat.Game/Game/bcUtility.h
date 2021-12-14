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
		
		if constexpr (graphic::bc_render_api_info::use_left_handed())
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

		if constexpr (graphic::bc_render_api_info::use_left_handed())
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

		if constexpr (graphic::bc_render_api_info::use_left_handed())
		{
			l_matrix.rotation_euler_lh(p_axis, p_radians);
		}
		else
		{
			l_matrix.rotation_euler_rh(p_axis, p_radians);
		}

		return l_matrix;
	}

	inline core::bc_matrix3f bc_matrix3f_rotation_zyx(const core::bc_vector3f& p_angles)
	{
		core::bc_matrix3f l_matrix;

		if constexpr (graphic::bc_render_api_info::use_left_handed())
		{
			l_matrix.rotation_zyx_lh(p_angles);
		}
		else
		{
			l_matrix.rotation_zyx_rh(p_angles);
		}

		return l_matrix;
	}

	inline core::bc_vector4f bc_matrix3f_decompose_to_axis_angle(const core::bc_matrix3f& p_transform)
	{
		const auto l_up = p_transform.get_row(1);
		const auto l_right = p_transform.get_row(0);
		const auto l_up_rotation = std::acos(core::bc_vector3f::dot(core::bc_vector3f::right(), l_right));

		return core::bc_vector4f(l_up, l_up_rotation);
	}
	
	inline core::bc_vector4f bc_matrix4f_decompose_to_axis_angle(const core::bc_matrix4f& p_transform)
	{
		const auto l_up = p_transform.get_row(1).xyz();
		const auto l_right = p_transform.get_row(0).xyz();
		const auto l_up_rotation = std::acos(core::bc_vector3f::dot(core::bc_vector3f::right(), l_right));

		return core::bc_vector4f(l_up, l_up_rotation);
	}

	inline core::bc_vector3f bc_matrix3f_decompose_to_euler_angles(const core::bc_matrix3f& p_transform)
	{
		https://stackoverflow.com/questions/15022630/how-to-calculate-the-angle-from-rotation-matrix
		const auto l_tx = std::atan2(p_transform(2, 1), p_transform(2, 2));
		const auto l_ty = std::atan2(-p_transform(2, 0), std::sqrtf(std::powf(p_transform(2, 1), 2) + std::powf(p_transform(2, 2), 2)));
		const auto l_tz = std::atan2(p_transform(1, 0), p_transform(0, 0));

		return { l_tx, l_ty, l_tz };
	}
	
	inline core::bc_vector3f bc_matrix4f_decompose_to_euler_angles(const core::bc_matrix4f& p_transform)
	{
		https://stackoverflow.com/questions/15022630/how-to-calculate-the-angle-from-rotation-matrix
		const auto l_tx = std::atan2(p_transform(2, 1), p_transform(2, 2));
		const auto l_ty = std::atan2(-p_transform(2, 0), std::sqrtf(std::powf(p_transform(2, 1), 2) + std::powf(p_transform(2, 2), 2)));
		const auto l_tz = std::atan2(p_transform(1, 0), p_transform(0, 0));

		return {l_tx, l_ty, l_tz};
	}

	inline core::bc_matrix4f bc_matrix4f_from_position_and_direction(const core::bc_vector3f& p_position, const core::bc_vector3f& p_direction)
	{
		core::bc_matrix4f l_matrix;
		l_matrix.make_identity();
		l_matrix.set_translation(p_position);
		l_matrix.set_rotation(bc_matrix3f_rotation_between_two_vector(core::bc_vector3f::forward(), p_direction));

		return l_matrix;
	}
}