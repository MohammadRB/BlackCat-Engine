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
		class BC_CORE_DLL bc_vector3f
		{
		public:
			bc_vector3f();

			bc_vector3f(bcFLOAT p_x, bcFLOAT p_y, bcFLOAT p_z);

			bc_vector3f(const bc_vector3f& p_other);

			// vector operations
			void clamp();

			bc_vector3f cross(const bc_vector3f& p_vector) const;

			bcFLOAT dot(const bc_vector3f& p_vector) const;

			void make_zero();

			bcFLOAT magnitude();

			void normalize();

			bc_vector3f perpendicular();

			// static vector ops
			static bc_vector3f clamp(const bc_vector3f& p_first);

			static bc_vector3f cross(const bc_vector3f& p_first, const bc_vector3f& p_second);

			static bcFLOAT dot(const bc_vector3f& p_first, const bc_vector3f& p_second);

			static bcFLOAT length_sq(const bc_vector3f& p_vector);

			static bc_vector3f normalize(const bc_vector3f& p_vector);

			static bc_vector3f perpendicular(const bc_vector3f& p_vector);

			static bc_vector3f random();
			
			// assignment
			bc_vector3f& operator= (const bc_vector3f& p_other);

			// member access
			bcFLOAT operator[] (int p_pos) const;

			bcFLOAT& operator[] (int p_pos);

			// comparison
			bool operator== (const bc_vector3f& p_other) const;

			bool operator!= (const bc_vector3f& p_other) const;

			// arithmetic operations
			bc_vector3f operator+ (const bc_vector3f& p_other) const;

			bc_vector3f operator- (const bc_vector3f& p_other) const;

			bc_vector3f operator* (const bc_vector3f& p_other) const;

			bc_vector3f operator* (bcFLOAT p_scalar) const;

			bc_vector3f operator/ (const bc_vector3f& p_other) const;

			bc_vector3f operator/ (bcFLOAT p_scalar) const;

			bc_vector3f operator- () const;

			// arithmetic updates
			bc_vector3f& operator+= (const bc_vector3f& p_other);

			bc_vector3f& operator-= (const bc_vector3f& p_other);

			bc_vector3f& operator*= (const bc_vector3f& p_other);

			bc_vector3f& operator*= (bcFLOAT p_scalar);

			bc_vector3f& operator/= (const bc_vector3f& p_other);

			bc_vector3f& operator/= (bcFLOAT p_scalar);

		public:
			bcFLOAT x;
			bcFLOAT y;
			bcFLOAT z;
		};
	}
}