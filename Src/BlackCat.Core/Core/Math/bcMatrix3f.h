#pragma once

#include "CorePlatform/bcType.h"
#include "Core/bcExport.h"
#include "Core/Math/bcVector3f.h"

namespace black_cat
{
	namespace core
	{
		class BC_CORE_DLL bc_matrix3f
		{
		public:
			bc_matrix3f() noexcept;

			explicit bc_matrix3f(bool p_zero) noexcept;

			bc_matrix3f(const bc_matrix3f& p_other) noexcept;

			bc_matrix3f(bcFLOAT p_m11, bcFLOAT p_m12, bcFLOAT p_m13,
				bcFLOAT p_m21, bcFLOAT p_m22, bcFLOAT p_m23,
				bcFLOAT p_m31, bcFLOAT p_m32, bcFLOAT p_m33) noexcept;

			bc_matrix3f& operator=(const bc_matrix3f& p_matrix) noexcept;

			void rotation_x_lh(bcFLOAT p_radians) noexcept;

			void rotation_y_lh(bcFLOAT p_radians) noexcept;

			void rotation_z_lh(bcFLOAT p_radians) noexcept;

			void rotation_lh(bc_vector3f& p_rot) noexcept;

			void rotation_zyx_lh(const bc_vector3f& p_rot) noexcept;

			void rotation_euler_lh(const bc_vector3f& p_axis, bcFLOAT p_radians) noexcept;

			void rotation_between_two_vector_lh(const bc_vector3f& p_v1, const bc_vector3f& p_v2) noexcept;
			
			void rotation_between_two_vector_checked_lh(const bc_vector3f& p_v1, const bc_vector3f& p_v2) noexcept;

			void rotation_x_rh(bcFLOAT p_radians) noexcept;

			void rotation_y_rh(bcFLOAT p_radians) noexcept;

			void rotation_z_rh(bcFLOAT p_radians) noexcept;

			void rotation_rh(const bc_vector3f& p_rot) noexcept;

			void rotation_zyx_rh(const bc_vector3f& p_rot) noexcept;

			void rotation_euler_rh(const bc_vector3f& p_axis, bcFLOAT p_radians) noexcept;

			void rotation_between_two_vector_rh(const bc_vector3f& p_v1, const bc_vector3f& p_v2) noexcept;
			
			void rotation_between_two_vector_checked_rh(const bc_vector3f& p_v1, const bc_vector3f& p_v2) noexcept;
			
			void orthonormalize() noexcept;

			void make_zero() noexcept;

			void make_identity() noexcept;

			void make_transpose() noexcept;

			void make_neutralize_scale() noexcept;

			bc_matrix3f transpose() const noexcept;

			void set_row(bcINT p_row, const bc_vector3f& p_vector) noexcept;

			bc_vector3f get_row(bcINT p_row) const noexcept;

			void set_column(bcINT p_col, const bc_vector3f& p_vector) noexcept;

			bc_vector3f get_column(bcINT p_col) const noexcept;

			bcFLOAT operator()(bcINT p_row, bcINT p_col) const noexcept;

			bcFLOAT& operator()(bcINT p_row, bcINT p_col) noexcept;

			bcFLOAT operator[](bcINT p_pos) const noexcept;

			bcFLOAT& operator[](bcINT p_pos) noexcept;

			bool operator==(const bc_matrix3f& p_other) const noexcept;

			bool operator!=(const bc_matrix3f& p_other) const noexcept;

			bc_matrix3f operator+(const bc_matrix3f& p_other) const noexcept;

			bc_matrix3f operator-(const bc_matrix3f& p_other) const noexcept;

			bc_matrix3f operator*(const bc_matrix3f& p_other) const noexcept;

			bc_matrix3f operator*(bcFLOAT p_scalar) const noexcept;

			bc_matrix3f operator/(bcFLOAT p_scalar) const noexcept;

			bc_matrix3f operator-() const noexcept;

			bc_matrix3f& operator+=(const bc_matrix3f& p_other) noexcept;

			bc_matrix3f& operator-=(const bc_matrix3f& p_other) noexcept;

			bc_matrix3f& operator*=(const bc_matrix3f& p_other) noexcept;

			bc_matrix3f& operator*=(bcFLOAT p_scalar) noexcept;

			bc_matrix3f& operator/=(bcFLOAT p_scalar) noexcept;

			bc_vector3f operator*(const bc_vector3f& p_vector) const noexcept;

			static bc_matrix3f zero() noexcept;

			static bc_matrix3f identity() noexcept;

			constexpr static bool use_column_major_storage()
			{
				return true;
			}

		protected:
			constexpr static bcUINT32 i(bcUINT32 p_index);
			
			constexpr static bcUINT32 i(bcUINT32 p_row, bcUINT32 p_col);

			bcFLOAT m_entry[3 * 3];
		};
	}
}