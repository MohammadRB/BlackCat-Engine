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
#include "Core/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_vector2f;
		class bc_vector3f;

		class BC_CORE_DLL bc_vector4f
		{
		public:
			bc_vector4f();

			bc_vector4f(float p_x, float p_y, float p_z, float p_w);

			bc_vector4f(const bc_vector3f& p_vector, float w);

			bc_vector4f(const bc_vector4f& p_vector);
			
			// vector operations
			void clamp();

			float dot(bc_vector4f& p_other) const;

			void make_zero();

			void normalize();

			float magnitude() const;

			// Operators
			bc_vector4f& operator= (const bc_vector4f& p_other);

			// member access
			float operator[] (int p_pos) const;

			float& operator[] (int p_pos);

			// comparison
			bool operator== (const bc_vector4f& p_other) const;

			bool operator!= (const bc_vector4f& p_other) const;

			// arithmetic operations
			bc_vector4f operator+ (const bc_vector4f& p_other) const;

			bc_vector4f operator- (const bc_vector4f& p_other) const;

			bc_vector4f operator* (float p_scalar) const;

			bc_vector4f operator* (const bc_vector4f& p_other) const;

			bc_vector4f operator/ (float p_scalar) const;

			bc_vector4f operator/ (const bc_vector4f& p_other) const;

			bc_vector4f operator- () const;

			// arithmetic updates
			bc_vector4f& operator+= (const bc_vector4f& p_other);

			bc_vector4f& operator-= (const bc_vector4f& p_other);

			bc_vector4f& operator*= (float p_scalar);

			bc_vector4f& operator*= (const bc_vector4f& p_other);

			bc_vector4f& operator/= (float p_scalar);

			bc_vector4f& operator/= (const bc_vector4f& p_other);

			unsigned int to_ARGB();

			unsigned int to_RGBA();

			void from_ARGB(unsigned int color);

			bc_vector3f xyz() const;

			bc_vector2f xy() const;

		public:
			float x;
			float y;
			float z;
			float w;
		};
	}
}