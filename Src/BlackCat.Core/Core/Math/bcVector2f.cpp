//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------

#include "Core/CorePCH.h"
#include "Core/Math/bcVector2f.h"

namespace black_cat
{
	namespace core
	{
		//bc_vector2f::bc_vector2f() : x(0), y(0)
		//{
		//}
		//
		//bc_vector2f::bc_vector2f(bcFLOAT p_x, bcFLOAT p_y)
		//{
		//	x = p_x;
		//	y = p_y;
		//}
		//
		//bc_vector2f::bc_vector2f(const bc_vector2f& p_vector)
		//{
		//	x = p_vector.x;
		//	y = p_vector.y;
		//}
		//
		//bc_vector2f& bc_vector2f::operator= (const bc_vector2f& p_vector)
		//{
		//	x = p_vector.x;
		//	y = p_vector.y;

		//	return(*this);
		//}

		//void bc_vector2f::clamp()
		//{
		//	if (x > 1.0f) x = 1.0f;
		//	if (x < 0.0f) x = 0.0f;

		//	if (y > 1.0f) y = 1.0f;
		//	if (y < 0.0f) y = 0.0f;
		//}
		//
		//void bc_vector2f::normalize()
		//{
		//	bcFLOAT l_inv_mag = (1.0f / magnitude());

		//	x *= l_inv_mag;
		//	y *= l_inv_mag;
		//}
		//
		//bcFLOAT bc_vector2f::magnitude()
		//{
		//	bcFLOAT l_length = 0.0f;

		//	l_length += x * x;
		//	l_length += y * y;

		//	return(sqrtf(l_length));
		//}

		//void bc_vector2f::make_zero()
		//{
		//	x = 0.0f;
		//	y = 0.0f;
		//}
		//
		//bcFLOAT bc_vector2f::operator[] (int p_pos) const
		//{
		//	if (p_pos == 0) return(x);
		//	return(y);
		//}
		//
		//bcFLOAT& bc_vector2f::operator[] (int p_pos)
		//{
		//	if (p_pos == 0) return(x);
		//	return(y);
		//}
		//
		//bool bc_vector2f::operator== (const bc_vector2f& p_vector) const
		//{
		//	if ((x - p_vector.x) * (x - p_vector.x) > 0.01f)
		//		return false;
		//	if ((y - p_vector.y) * (y - p_vector.y) > 0.01f)
		//		return false;

		//	return(true);
		//}
		//
		//bool bc_vector2f::operator!= (const bc_vector2f& p_vector) const
		//{
		//	return(!(*this == p_vector));
		//}
		//
		//bc_vector2f bc_vector2f::operator+ (const bc_vector2f& p_vector) const
		//{
		//	bc_vector2f l_sum;

		//	l_sum.x = x + p_vector.x;
		//	l_sum.y = y + p_vector.y;

		//	return(l_sum);
		//}
		//
		//bc_vector2f bc_vector2f::operator- (const bc_vector2f& p_vector) const
		//{
		//	bc_vector2f l_diff;

		//	l_diff.x = x - p_vector.x;
		//	l_diff.y = y - p_vector.y;

		//	return(l_diff);
		//}
		//
		//bc_vector2f bc_vector2f::operator* (const bc_vector2f& p_vector) const
		//{
		//	bc_vector2f l_prod;

		//	l_prod.x = x * p_vector.x;
		//	l_prod.y = y * p_vector.y;

		//	return(l_prod);
		//}
		//
		//bc_vector2f bc_vector2f::operator* (bcFLOAT p_scalar) const
		//{
		//	bc_vector2f l_prod;

		//	l_prod.x = x * p_scalar;
		//	l_prod.y = y * p_scalar;

		//	return(l_prod);
		//}
		//
		//bc_vector2f bc_vector2f::operator/ (bcFLOAT p_scalar) const
		//{
		//	bc_vector2f l_quot;
		//	if (p_scalar != 0.0f)
		//	{
		//		bcFLOAT l_inv_scalar = 1.0f / p_scalar;
		//		l_quot.x = x * l_inv_scalar;
		//		l_quot.y = y * l_inv_scalar;
		//	}
		//	else
		//	{
		//		l_quot.make_zero();
		//	}

		//	return(l_quot);
		//}
		//
		//bc_vector2f bc_vector2f::operator- () const
		//{
		//	bc_vector2f l_neg;

		//	l_neg.x = -x;
		//	l_neg.y = -y;

		//	return(l_neg);
		//}
		//
		//bc_vector2f& bc_vector2f::operator+= (const bc_vector2f& p_vector)
		//{
		//	x += p_vector.x;
		//	y += p_vector.y;

		//	return(*this);
		//}
		//
		//bc_vector2f& bc_vector2f::operator-= (const bc_vector2f& p_vector)
		//{
		//	x -= p_vector.x;
		//	y -= p_vector.y;

		//	return(*this);
		//}
		//
		//bc_vector2f& bc_vector2f::operator*= (bcFLOAT p_scalar)
		//{
		//	x *= p_scalar;
		//	y *= p_scalar;

		//	return(*this);
		//}
		//
		//bc_vector2f& bc_vector2f::operator/= (bcFLOAT p_scalar)
		//{
		//	if (p_scalar != 0.0f)
		//	{
		//		bcFLOAT l_inv_scalar = 1.0f / p_scalar;
		//		x *= l_inv_scalar;
		//		y *= l_inv_scalar;
		//	}
		//	else
		//	{
		//		make_zero();
		//	}

		//	return(*this);
		//}
	}
}