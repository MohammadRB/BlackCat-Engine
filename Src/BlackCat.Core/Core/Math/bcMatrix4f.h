//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------

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

			bc_matrix4f();

			bc_matrix4f(bool p_zero);

			bc_matrix4f(const bc_matrix4f& p_other);

			bc_matrix4f(bcFLOAT p_m11, bcFLOAT p_m12, bcFLOAT p_m13, bcFLOAT p_m14,
				bcFLOAT p_m21, bcFLOAT p_m22, bcFLOAT p_m23, bcFLOAT p_m24,
				bcFLOAT p_m31, bcFLOAT p_m32, bcFLOAT p_m33, bcFLOAT p_m34,
				bcFLOAT p_m41, bcFLOAT p_m42, bcFLOAT p_m43, bcFLOAT p_m44);

			bc_matrix4f& operator= (const bc_matrix4f& p_matrix);
			
			void rotation_x_lh(bcFLOAT p_radians);

			void rotation_y_lh(bcFLOAT p_radians);

			void rotation_z_lh(bcFLOAT p_radians);

			void rotation_x_rh(bcFLOAT p_radians);

			void rotation_y_rh(bcFLOAT p_radians);

			void rotation_z_rh(bcFLOAT p_radians);

			void scale(bcFLOAT p_scale);

			void translate(bcFLOAT p_x, bcFLOAT p_y, bcFLOAT p_z);

			bc_vector3f get_basis_x() const;

			bc_vector3f get_basis_y() const;

			bc_vector3f get_basis_z() const;

			bc_vector3f get_translation() const;

			bc_matrix3f get_rotation() const;

			void set_rotation(const bc_matrix3f& p_rot);

			void set_translation(const bc_vector3f& p_trans);

			bc_matrix4f inverse() const;

			void make_zero();

			void make_identity();

			void make_transpose();

			static bc_matrix4f zero();

			static bc_matrix4f identity();

			bc_matrix4f transpose() const;
			
			void set_row(bcINT p_row, const bc_vector4f& p_vector);

			void set_row(bcINT p_row, const bc_vector3f& p_vector);

			bc_vector4f get_row(bcINT p_row) const;

			void set_column(bcINT p_col, const bc_vector4f& p_vector);

			bc_vector4f get_column(bcINT p_col) const;

			bcFLOAT operator() (bcINT p_row, bcINT p_col) const;

			bcFLOAT& operator() (bcINT p_row, bcINT p_col);

			bcFLOAT operator[] (bcINT p_pos) const;

			bcFLOAT& operator[] (bcINT p_pos);

			bool operator== (const bc_matrix4f& p_other) const;

			bool operator!= (const bc_matrix4f& p_other) const;

			bc_matrix4f operator+ (const bc_matrix4f& p_other) const;

			bc_matrix4f operator- (const bc_matrix4f& p_other) const;

			bc_matrix4f operator* (const bc_matrix4f& p_other) const;

			bc_matrix4f operator* (bcFLOAT p_scalar) const;

			bc_matrix4f operator/ (bcFLOAT p_scalar) const;

			bc_matrix4f operator- () const;

			bc_matrix4f& operator+= (const bc_matrix4f& p_other);

			bc_matrix4f& operator-= (const bc_matrix4f& p_other);

			bc_matrix4f& operator*= (const bc_matrix4f& p_other);

			bc_matrix4f& operator*= (bcFLOAT p_scalar);

			bc_matrix4f& operator/= (bcFLOAT p_scalar);

			bc_vector4f operator* (const bc_vector4f& p_other) const;

			static bc_matrix4f rotation_matrix_xyz_lh(bcFLOAT p_radians_x, bcFLOAT p_radians_y, bcFLOAT p_radians_z);

			static bc_matrix4f rotation_matrix_x_lh(bcFLOAT p_radians);

			static bc_matrix4f rotation_matrix_y_lh(bcFLOAT p_radians);
			
			static bc_matrix4f rotation_matrix_z_lh(bcFLOAT p_radians);

			static bc_matrix4f rotation_matrix_xyz_rh(bcFLOAT p_radians_x, bcFLOAT p_radians_y, bcFLOAT p_radians_z);

			static bc_matrix4f rotation_matrix_x_rh(bcFLOAT p_radians);

			static bc_matrix4f rotation_matrix_y_rh(bcFLOAT p_radians);

			static bc_matrix4f rotation_matrix_z_rh(bcFLOAT p_radians);

			static bc_matrix4f scale_matrix(bcFLOAT p_scale);

			static bc_matrix4f scale_matrix(const bc_vector3f& p_scale);

			static bc_matrix4f scale_matrix_xyz(bcFLOAT p_x, bcFLOAT p_y, bcFLOAT p_z);

			static bc_matrix4f translation_matrix(bcFLOAT p_x, bcFLOAT p_y, bcFLOAT p_z);

			static bc_matrix4f look_at_matrix_lh(const bc_vector3f& p_eye, const bc_vector3f& p_at, const bc_vector3f& p_up);

			static bc_matrix4f perspective_fov_matrix_lh(bcFLOAT p_fovy, bcFLOAT p_aspect, bcFLOAT p_zn, bcFLOAT p_zf);

			static bc_matrix4f orthographic_matrix_lh(bcFLOAT p_zn, bcFLOAT p_zf, bcFLOAT p_width, bcFLOAT p_height);

			static bc_matrix4f look_at_matrix_rh(const bc_vector3f& p_eye, const bc_vector3f& p_at, const bc_vector3f& p_up);

			static bc_matrix4f perspective_fov_matrix_rh(bcFLOAT p_fovy, bcFLOAT p_aspect, bcFLOAT p_zn, bcFLOAT p_zf);

			static bc_matrix4f orthographic_matrix_rh(bcFLOAT p_zn, bcFLOAT p_zf, bcFLOAT p_width, bcFLOAT p_height);

			static const bcINT m11 = 0;
			static const bcINT m12 = 1;
			static const bcINT m13 = 2;
			static const bcINT m14 = 3;

			static const bcINT m21 = 4;
			static const bcINT m22 = 5;
			static const bcINT m23 = 6;
			static const bcINT m24 = 7;

			static const bcINT m31 = 8;
			static const bcINT m32 = 9;
			static const bcINT m33 = 10;
			static const bcINT m34 = 11;

			static const bcINT m41 = 12;
			static const bcINT m42 = 13;
			static const bcINT m43 = 14;
			static const bcINT m44 = 15;
			
		protected:
			bcFLOAT m_entry[4 * 4];

			static bcINT I(bcINT iRow, bcINT iCol); // iRow*N + iCol
		};
	}
}
