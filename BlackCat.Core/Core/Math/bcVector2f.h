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

			void clamp();

			void normalize();

			bcFLOAT magnitude();

			void make_zero();

			bc_vector2f& operator= (const bc_vector2f& p_other);

			bcFLOAT operator[] (int p_pos) const;

			bcFLOAT& operator[] (int p_pos);

			bool operator== (const bc_vector2f& p_other) const;

			bool operator!= (const bc_vector2f& p_other) const;

			bc_vector2f operator+ (const bc_vector2f& p_other) const;

			bc_vector2f operator- (const bc_vector2f& p_other) const;

			bc_vector2f operator* (const bc_vector2f& p_other) const;

			bc_vector2f operator* (bcFLOAT p_scalar) const;

			bc_vector2f operator/ (bcFLOAT p_scalar) const;

			bc_vector2f operator- () const;

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