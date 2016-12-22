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
		class BC_CORE_DLL bc_vector2f
		{
		public:
			bc_vector2f();

			bc_vector2f(bcFLOAT x, bcFLOAT y);

			bc_vector2f(const bc_vector2f& p_other);

			// vector operations
			void clamp();

			void make_zero();

			void normalize();

			bcFLOAT magnitude();

			// assignment
			bc_vector2f& operator= (const bc_vector2f& p_other);

			// accessors
			bcFLOAT operator[] (int p_pos) const;

			bcFLOAT& operator[] (int p_pos);

			// boolean comparison
			bool operator== (const bc_vector2f& p_other) const;

			bool operator!= (const bc_vector2f& p_other) const;

			// arithmetic operations
			bc_vector2f operator+ (const bc_vector2f& p_other) const;

			bc_vector2f operator- (const bc_vector2f& p_other) const;

			bc_vector2f operator* (const bc_vector2f& p_other) const;

			bc_vector2f operator* (bcFLOAT p_scalar) const;

			bc_vector2f operator/ (bcFLOAT p_scalar) const;

			bc_vector2f operator- () const;

			// arithmetic updates
			bc_vector2f& operator+= (const bc_vector2f& p_other);

			bc_vector2f& operator-= (const bc_vector2f& p_other);

			bc_vector2f& operator*= (bcFLOAT p_scalar);

			bc_vector2f& operator/= (bcFLOAT p_scalar);

		public:
			bcFLOAT x;
			bcFLOAT y;
		};
	}
}