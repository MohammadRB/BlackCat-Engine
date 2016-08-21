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
#include "Graphic/bcExport.h"
#include "Graphic/Math/bcMatrix3f.h"

namespace black_cat
{
	namespace graphic
	{
		class bc_vector3f;

		class BC_GRAPHIC_DLL bc_matrix3f
		{
		public:
			bc_matrix3f();

			explicit bc_matrix3f(bool p_zero);

			bc_matrix3f(const bc_matrix3f& p_other);

			bc_matrix3f(bcFLOAT p_m11, bcFLOAT p_m12, bcFLOAT p_m13,
				bcFLOAT p_m21, bcFLOAT p_m22, bcFLOAT p_m23,
				bcFLOAT p_m31, bcFLOAT p_m32, bcFLOAT p_m33);

			void rotation_x(bcFLOAT p_radians);

			void rotation_y(bcFLOAT p_radians);

			void rotation_z(bcFLOAT p_radians);

			void rotation(bc_vector3f& p_rot);

			void rotation_zyx(bc_vector3f& p_rot);

			void rotation_euler(bc_vector3f& p_axis, bcFLOAT p_angle);

			void orthonormalize();

			void make_zero();

			void make_identity();

			void make_transpose();

			static bc_matrix3f zero();

			static bc_matrix3f identity();

			bc_matrix3f transpose();

			// Operators
			bc_matrix3f& operator= (const bc_matrix3f& p_matrix);

			// member access
			bcFLOAT operator() (bcINT p_row, bcINT p_col) const;

			bcFLOAT& operator() (bcINT p_row, bcINT p_col);

			bcFLOAT operator[] (bcINT p_pos) const;

			bcFLOAT& operator[] (bcINT p_pos);

			void set_row(bcINT p_row, const bc_vector3f& p_vector);

			bc_vector3f get_row(bcINT p_row) const;

			void set_column(bcINT p_col, const bc_vector3f& p_vector);

			bc_vector3f get_column(bcINT p_col) const;

			// comparison
			bool operator== (const bc_matrix3f& p_other) const;

			bool operator!= (const bc_matrix3f& p_other) const;

			// arithmetic operations
			bc_matrix3f operator+ (const bc_matrix3f& p_other) const;

			bc_matrix3f operator- (const bc_matrix3f& p_other) const;

			bc_matrix3f operator* (const bc_matrix3f& p_other) const;

			bc_matrix3f operator* (bcFLOAT p_scalar) const;

			bc_matrix3f operator/ (bcFLOAT p_scalar) const;

			bc_matrix3f operator- () const;

			// arithmetic updates
			bc_matrix3f& operator+= (const bc_matrix3f& p_other);

			bc_matrix3f& operator-= (const bc_matrix3f& p_other);

			bc_matrix3f& operator*= (const bc_matrix3f& p_other);

			bc_matrix3f& operator*= (bcFLOAT p_scalar);

			bc_matrix3f& operator/= (bcFLOAT p_scalar);

			// matrix - vector operations
			bc_vector3f operator* (const bc_vector3f& p_vector) const;  // M * v

		protected:
			bcFLOAT m_entry[3 * 3];

			static bcINT I(bcINT p_row, bcINT p_col); // p_row*N + p_col
		};
	}
}