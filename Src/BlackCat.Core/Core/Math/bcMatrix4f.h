#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcVector4f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class BC_CORE_DLL bc_matrix4f
		{
		public:
			bc_matrix4f() noexcept;

			bc_matrix4f(bool p_zero) noexcept;

			bc_matrix4f(const bc_matrix4f& p_other) noexcept;

			bc_matrix4f(bcFLOAT p_m11, bcFLOAT p_m12, bcFLOAT p_m13, bcFLOAT p_m14,
				bcFLOAT p_m21, bcFLOAT p_m22, bcFLOAT p_m23, bcFLOAT p_m24,
				bcFLOAT p_m31, bcFLOAT p_m32, bcFLOAT p_m33, bcFLOAT p_m34,
				bcFLOAT p_m41, bcFLOAT p_m42, bcFLOAT p_m43, bcFLOAT p_m44) noexcept;

			bc_matrix4f& operator= (const bc_matrix4f& p_other) noexcept;
			
			void rotation_x_lh(bcFLOAT p_radians) noexcept;

			void rotation_y_lh(bcFLOAT p_radians) noexcept;

			void rotation_z_lh(bcFLOAT p_radians) noexcept;

			void rotation_x_rh(bcFLOAT p_radians) noexcept;

			void rotation_y_rh(bcFLOAT p_radians) noexcept;

			void rotation_z_rh(bcFLOAT p_radians) noexcept;

			void scale(bcFLOAT p_scale) noexcept;

			void translate(bcFLOAT p_x, bcFLOAT p_y, bcFLOAT p_z) noexcept;
			
			void translate(const bc_vector3f& p_translate) noexcept;

			bc_vector3f get_basis_x() const noexcept;

			bc_vector3f get_basis_y() const noexcept;

			bc_vector3f get_basis_z() const noexcept;

			bc_vector3f get_translation() const noexcept;

			bc_matrix3f get_rotation() const noexcept;

			void set_rotation(const bc_matrix3f& p_rot) noexcept;

			void set_translation(const bc_vector3f& p_trans) noexcept;

			bc_matrix4f inverse() const noexcept;

			void make_zero() noexcept;

			void make_identity() noexcept;

			void make_transpose() noexcept;

			void make_neutralize_scale() noexcept;

			bc_matrix4f transpose() const noexcept;
			
			void set_row(bcINT p_row, const bc_vector4f& p_vector) noexcept;

			void set_row(bcINT p_row, const bc_vector3f& p_vector) noexcept;

			bc_vector4f get_row(bcINT p_row) const noexcept;

			void set_column(bcINT p_col, const bc_vector4f& p_vector) noexcept;

			bc_vector4f get_column(bcINT p_col) const noexcept;

			bcFLOAT operator() (bcINT p_row, bcINT p_col) const noexcept;

			bcFLOAT& operator() (bcINT p_row, bcINT p_col) noexcept;

			bcFLOAT operator[] (bcINT p_pos) const noexcept;

			bcFLOAT& operator[] (bcINT p_pos) noexcept;

			bool operator== (const bc_matrix4f& p_other) const noexcept;

			bool operator!= (const bc_matrix4f& p_other) const noexcept;

			bc_matrix4f operator+ (const bc_matrix4f& p_other) const noexcept;

			bc_matrix4f operator- (const bc_matrix4f& p_other) const noexcept;

			bc_matrix4f operator* (const bc_matrix4f& p_other) const noexcept;

			bc_matrix4f operator* (bcFLOAT p_scalar) const noexcept;

			bc_matrix4f operator/ (bcFLOAT p_scalar) const noexcept;

			bc_matrix4f operator- () const noexcept;

			bc_matrix4f& operator+= (const bc_matrix4f& p_other) noexcept;

			bc_matrix4f& operator-= (const bc_matrix4f& p_other) noexcept;

			bc_matrix4f& operator*= (const bc_matrix4f& p_other) noexcept;

			bc_matrix4f& operator*= (bcFLOAT p_scalar) noexcept;

			bc_matrix4f& operator/= (bcFLOAT p_scalar) noexcept;

			bc_vector4f operator* (const bc_vector4f& p_vector) const noexcept;

			static bc_matrix4f rotation_matrix_xyz_lh(bcFLOAT p_radians_x, bcFLOAT p_radians_y, bcFLOAT p_radians_z) noexcept;

			static bc_matrix4f rotation_matrix_x_lh(bcFLOAT p_radians) noexcept;

			static bc_matrix4f rotation_matrix_y_lh(bcFLOAT p_radians) noexcept;
			
			static bc_matrix4f rotation_matrix_z_lh(bcFLOAT p_radians) noexcept;

			static bc_matrix4f rotation_matrix_xyz_rh(bcFLOAT p_radians_x, bcFLOAT p_radians_y, bcFLOAT p_radians_z) noexcept;

			static bc_matrix4f rotation_matrix_x_rh(bcFLOAT p_radians) noexcept;

			static bc_matrix4f rotation_matrix_y_rh(bcFLOAT p_radians) noexcept;

			static bc_matrix4f rotation_matrix_z_rh(bcFLOAT p_radians) noexcept;

			static bc_matrix4f scale_matrix(bcFLOAT p_scale) noexcept;

			static bc_matrix4f scale_matrix(const bc_vector3f& p_scale) noexcept;

			static bc_matrix4f scale_matrix_xyz(bcFLOAT p_x, bcFLOAT p_y, bcFLOAT p_z) noexcept;
			
			static bc_matrix4f translation_matrix(bcFLOAT p_x, bcFLOAT p_y, bcFLOAT p_z) noexcept;
			
			static bc_matrix4f translation_matrix(const bc_vector3f& p_translation) noexcept;

			static bc_matrix4f look_at_matrix_lh(const bc_vector3f& p_eye, const bc_vector3f& p_at, const bc_vector3f& p_up) noexcept;

			static bc_matrix4f perspective_fov_matrix_lh(bcFLOAT p_fovy, bcFLOAT p_aspect, bcFLOAT p_zn, bcFLOAT p_zf) noexcept;

			static bc_matrix4f orthographic_matrix_lh(bcFLOAT p_zn, bcFLOAT p_zf, bcFLOAT p_width, bcFLOAT p_height) noexcept;

			static bc_matrix4f look_at_matrix_rh(const bc_vector3f& p_eye, const bc_vector3f& p_at, const bc_vector3f& p_up) noexcept;

			static bc_matrix4f perspective_fov_matrix_rh(bcFLOAT p_fovy, bcFLOAT p_aspect, bcFLOAT p_zn, bcFLOAT p_zf) noexcept;

			static bc_matrix4f orthographic_matrix_rh(bcFLOAT p_zn, bcFLOAT p_zf, bcFLOAT p_width, bcFLOAT p_height) noexcept;

			static bc_matrix4f zero() noexcept;

			static bc_matrix4f identity() noexcept;

			constexpr static bool use_column_major_storage()
			{
				return true;
			}

		protected:
			constexpr static bcUINT32 i(bcUINT32 p_index);

			constexpr static bcUINT32 i(bcUINT32 p_row, bcUINT32 p_col);
			
			bcFLOAT m_entry[4 * 4];
		};
	}
}