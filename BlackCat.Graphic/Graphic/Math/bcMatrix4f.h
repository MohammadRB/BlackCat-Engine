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

#include "Graphic/GraphicPCH.h"

namespace black_cat
{
	namespace graphic
	{
		class bc_vector3f;
		class bc_vector4f;
		class bc_matrix3f;

		class BC_GRAPHIC_DLL_EXP bc_matrix4f
		{
		public:

			bc_matrix4f();

			bc_matrix4f(bool p_zero);

			bc_matrix4f(const bc_matrix4f& p_other);

			bc_matrix4f(bcFLOAT32 p_m11, bcFLOAT32 p_m12, bcFLOAT32 p_m13, bcFLOAT32 p_m14,
				bcFLOAT32 p_m21, bcFLOAT32 p_m22, bcFLOAT32 p_m23, bcFLOAT32 p_m24,
				bcFLOAT32 p_m31, bcFLOAT32 p_m32, bcFLOAT32 p_m33, bcFLOAT32 p_m34,
				bcFLOAT32 p_m41, bcFLOAT32 p_m42, bcFLOAT32 p_m43, bcFLOAT32 p_m44);

			bc_matrix4f inverse() const;
			
			void rotation_x(bcFLOAT32 p_radians);

			void rotation_y(bcFLOAT32 p_radians);

			void rotation_z(bcFLOAT32 p_radians);

			void scale(bcFLOAT32 p_scale);

			void translate(bcFLOAT32 p_x, bcFLOAT32 p_y, bcFLOAT32 p_z);

			bc_vector3f get_basis_x() const;

			bc_vector3f get_basis_y() const;

			bc_vector3f get_basis_z() const;

			bc_vector3f get_translation() const;

			bc_matrix3f get_rotation() const;

			void set_rotation(const bc_matrix3f& p_rot);

			void set_translation(const bc_vector3f& p_trans);

			static bc_matrix4f rotation_matrix_xyz(bcFLOAT32 p_radians_x, bcFLOAT32 p_radians_y, bcFLOAT32 p_radians_z);

			static bc_matrix4f rotation_matrix_x(bcFLOAT32 p_radians);

			static bc_matrix4f rotation_matrix_y(bcFLOAT32 p_radians);

			static bc_matrix4f rotation_matrix_z(bcFLOAT32 p_radians);

			static bc_matrix4f scale_matrix(bcFLOAT32 p_scale);

			static bc_matrix4f scale_matrix(const bc_vector3f& p_scale);

			static bc_matrix4f scale_matrix_xyz(bcFLOAT32 p_x, bcFLOAT32 p_y, bcFLOAT32 p_z);

			static bc_matrix4f translation_matrix(bcFLOAT32 p_x, bcFLOAT32 p_y, bcFLOAT32 p_z);

			static bc_matrix4f look_at_lh_matrix(bc_vector3f& p_eye, bc_vector3f& p_at, bc_vector3f& p_up);

			static bc_matrix4f perspective_fov_lh_matrix(bcFLOAT32 p_fovy, bcFLOAT32 p_aspect, bcFLOAT32 p_zn, bcFLOAT32 p_zf);

			static bc_matrix4f orthographic_lh_matrix(bcFLOAT32 p_zn, bcFLOAT32 p_zf, bcFLOAT32 p_width, bcFLOAT32 p_height);

			void make_zero();

			void make_identity();

			void make_transpose();

			static bc_matrix4f zero();

			static bc_matrix4f identity();

			bc_matrix4f transpose();

			// Operators
			bc_matrix4f& operator= (const bc_matrix4f& p_matrix);

			// member access
			bcFLOAT32 operator() (bcINT p_row, bcINT p_col) const;

			bcFLOAT32& operator() (bcINT p_row, bcINT p_col);

			bcFLOAT32 operator[] (bcINT p_pos) const;

			bcFLOAT32& operator[] (bcINT p_pos);
			
			void set_row(bcINT p_row, const bc_vector4f& p_vector);

			void set_row(bcINT p_row, const bc_vector3f& p_vector);

			bc_vector4f get_row(bcINT p_row) const;

			void set_column(bcINT p_col, const bc_vector4f& p_vector);

			bc_vector4f get_column(bcINT p_col) const;

			// comparison
			bool operator== (const bc_matrix4f& p_other) const;

			bool operator!= (const bc_matrix4f& p_other) const;

			// arithmetic operations
			bc_matrix4f operator+ (const bc_matrix4f& p_other) const;

			bc_matrix4f operator- (const bc_matrix4f& p_other) const;

			bc_matrix4f operator* (const bc_matrix4f& p_other) const;

			bc_matrix4f operator* (bcFLOAT32 p_scalar) const;

			bc_matrix4f operator/ (bcFLOAT32 p_scalar) const;

			bc_matrix4f operator- () const;

			// arithmetic updates
			bc_matrix4f& operator+= (const bc_matrix4f& p_other);

			bc_matrix4f& operator-= (const bc_matrix4f& p_other);

			bc_matrix4f& operator*= (const bc_matrix4f& p_other);

			bc_matrix4f& operator*= (bcFLOAT32 p_scalar);

			bc_matrix4f& operator/= (bcFLOAT32 p_scalar);

			// matrix - vector operations
			bc_vector4f operator* (const bc_vector4f& p_other) const;  // M * v

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
			bcFLOAT32 m_entry[4 * 4];

			static bcINT I(bcINT iRow, bcINT iCol); // iRow*N + iCol
		};
	}
}