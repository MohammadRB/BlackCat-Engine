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
#include "Core/Math/bcVector3f.h"

namespace black_cat
{
	namespace core
	{
		bc_vector3f::bc_vector3f() : x(0), y(0), z(0)
		{
		}
		
		bc_vector3f::bc_vector3f(bcFLOAT p_x, bcFLOAT p_y, bcFLOAT p_z)
		{
			x = p_x;
			y = p_y;
			z = p_z;
		}
		
		bc_vector3f::bc_vector3f(const bc_vector3f& p_other)
		{
			x = p_other.x;
			y = p_other.y;
			z = p_other.z;
		}

		void bc_vector3f::clamp()
		{
			if (x > 1.0f) x = 1.0f;
			if (x < 0.0f) x = 0.0f;

			if (y > 1.0f) y = 1.0f;
			if (y < 0.0f) y = 0.0f;

			if (z > 1.0f) z = 1.0f;
			if (z < 0.0f) z = 0.0f;
		}

		bcFLOAT bc_vector3f::dot(const bc_vector3f& p_other) const
		{
			bcFLOAT l_ret = 0.0f;

			l_ret = x * p_other.x;
			l_ret += y * p_other.y;
			l_ret += z * p_other.z;

			return l_ret;
		}

		bc_vector3f bc_vector3f::cross(const bc_vector3f& p_other) const
		{
			bc_vector3f l_ret;

			l_ret.x = y * p_other.z - z * p_other.y;
			l_ret.y = z * p_other.x - x * p_other.z;
			l_ret.z = x * p_other.y - y * p_other.x;

			return(l_ret);
		}
		
		bcFLOAT bc_vector3f::magnitude()
		{
			bcFLOAT l_length = 0.0f;

			l_length += x * x;
			l_length += y * y;
			l_length += z * z;

			return(sqrt(l_length));
		}

		void bc_vector3f::normalize()
		{
			bcFLOAT l_mag = magnitude();
			if (0.0f == l_mag) l_mag = 0.0001f;

			bcFLOAT l_inv_mag = (1.0f / l_mag);

			x *= l_inv_mag;
			y *= l_inv_mag;
			z *= l_inv_mag;
		}

		bc_vector3f bc_vector3f::perpendicular()
		{
			bcFLOAT l_x_abs = fabs(x);
			bcFLOAT l_y_abs = fabs(y);
			bcFLOAT l_z_abs = fabs(z);
			bcFLOAT minVal = std::min(std::min(l_x_abs, l_y_abs), l_z_abs);

			if (l_x_abs == minVal)
				return cross(bc_vector3f(1.0f, 0.0f, 0.0f));
			else if (l_y_abs == minVal)
				return cross(bc_vector3f(0.0f, 1.0f, 0.0f));
			else
				return cross(bc_vector3f(0.0f, 0.0f, 1.0f));
		}

		void bc_vector3f::make_zero()
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
		}
		
		bc_vector3f& bc_vector3f::operator= (const bc_vector3f& p_other)
		{
			x = p_other.x;
			y = p_other.y;
			z = p_other.z;

			return(*this);
		}
		
		bcFLOAT bc_vector3f::operator[] (int p_pos) const
		{
			if (p_pos == 0) return(x);
			if (p_pos == 1) return(y);
			return(z);
		}
		
		bcFLOAT& bc_vector3f::operator[] (int p_pos)
		{
			if (p_pos == 0) return(x);
			if (p_pos == 1) return(y);
			return(z);
		}
		
		bool bc_vector3f::operator== (const bc_vector3f& p_other) const
		{

			if ((x - p_other.x) * (x - p_other.x) > 0.01f)
				return false;
			if ((y - p_other.y) * (y - p_other.y) > 0.01f)
				return false;
			if ((z - p_other.z) * (z - p_other.z) > 0.01f)
				return false;

			return true;
		}
		
		bool bc_vector3f::operator!= (const bc_vector3f& p_other) const
		{
			return(!(*this == p_other));
		}
		
		bc_vector3f bc_vector3f::operator+ (const bc_vector3f& p_other) const
		{
			bc_vector3f l_sum;

			l_sum.x = x + p_other.x;
			l_sum.y = y + p_other.y;
			l_sum.z = z + p_other.z;

			return(l_sum);
		}
		
		bc_vector3f bc_vector3f::operator- (const bc_vector3f& p_other) const
		{
			bc_vector3f l_diff;

			l_diff.x = x - p_other.x;
			l_diff.y = y - p_other.y;
			l_diff.z = z - p_other.z;

			return(l_diff);
		}
		
		bc_vector3f bc_vector3f::operator* (bcFLOAT p_scalar) const
		{
			bc_vector3f l_prod;

			l_prod.x = x * p_scalar;
			l_prod.y = y * p_scalar;
			l_prod.z = z * p_scalar;

			return(l_prod);
		}
		
		bc_vector3f bc_vector3f::operator* (const bc_vector3f& p_vector) const
		{
			bc_vector3f l_prod;

			l_prod.x = x * p_vector.x;
			l_prod.y = y * p_vector.y;
			l_prod.z = z * p_vector.z;

			return(l_prod);
		}
		
		bc_vector3f bc_vector3f::operator/ (bcFLOAT p_scalar) const
		{
			bc_vector3f l_quot;
			if (p_scalar != 0.0f)
			{
				bcFLOAT l_inv_scalar = 1.0f / p_scalar;
				l_quot.x = x * l_inv_scalar;
				l_quot.y = y * l_inv_scalar;
				l_quot.z = z * l_inv_scalar;
			}
			else
			{
				l_quot.make_zero();
			}

			return(l_quot);
		}
		
		bc_vector3f bc_vector3f::operator/ (const bc_vector3f& p_other) const
		{
			bc_vector3f l_quot;
			l_quot.x = p_other.x != 0.0f ? x / p_other.x : 0.0f;
			l_quot.y = p_other.y != 0.0f ? y / p_other.y : 0.0f;
			l_quot.z = p_other.z != 0.0f ? z / p_other.z : 0.0f;

			return(l_quot);
		}
		
		bc_vector3f bc_vector3f::operator- () const
		{
			bc_vector3f l_neg;

			l_neg.x = -x;
			l_neg.y = -y;
			l_neg.z = -z;

			return(l_neg);
		}
		
		bc_vector3f& bc_vector3f::operator+= (const bc_vector3f& p_other)
		{
			x += p_other.x;
			y += p_other.y;
			z += p_other.z;

			return(*this);
		}
		
		bc_vector3f& bc_vector3f::operator-= (const bc_vector3f& p_other)
		{
			x -= p_other.x;
			y -= p_other.y;
			z -= p_other.z;

			return(*this);
		}
		
		bc_vector3f& bc_vector3f::operator*= (bcFLOAT p_scalar)
		{
			x *= p_scalar;
			y *= p_scalar;
			z *= p_scalar;

			return(*this);
		}
		
		bc_vector3f& bc_vector3f::operator*= (const bc_vector3f& p_other)
		{
			x *= p_other.x;
			y *= p_other.y;
			z *= p_other.z;

			return(*this);
		}
		
		bc_vector3f& bc_vector3f::operator/= (bcFLOAT p_scalar)
		{
			if (p_scalar != 0.0f)
			{
				bcFLOAT l_inv_scalar = 1.0f / p_scalar;
				x *= l_inv_scalar;
				y *= l_inv_scalar;
				z *= l_inv_scalar;
			}
			else
			{
				make_zero();
			}

			return(*this);
		}
		
		bc_vector3f& bc_vector3f::operator/= (const bc_vector3f& p_other)
		{
			x = p_other.x != 0.0f ? x / p_other.x : 0.0f;
			y = p_other.y != 0.0f ? y / p_other.y : 0.0f;
			z = p_other.z != 0.0f ? z / p_other.z : 0.0f;

			return(*this);
		}
		
		bc_vector3f bc_vector3f::clamp(const bc_vector3f& p_vector)
		{
			bc_vector3f vec = p_vector;
			vec.clamp();
			return vec;
		}
		
		bc_vector3f bc_vector3f::cross(const bc_vector3f& p_first, const bc_vector3f& p_second)
		{
			return p_first.cross(p_second);
		}
		
		bcFLOAT bc_vector3f::dot(const bc_vector3f& p_first, const bc_vector3f& p_second)
		{
			return p_first.dot(p_second);
		}
		
		bcFLOAT bc_vector3f::length_sq(const bc_vector3f& p_vector)
		{
			return dot(p_vector, p_vector);
		}
		
		bc_vector3f bc_vector3f::normalize(const bc_vector3f& p_vector)
		{
			bc_vector3f l_vec = p_vector;
			l_vec.normalize();
			return l_vec;
		}
		
		bc_vector3f bc_vector3f::perpendicular(const bc_vector3f& p_vector)
		{
			bc_vector3f vec = p_vector;
			return vec.perpendicular();
		}

		bc_vector3f bc_vector3f::random()
		{
			bcFLOAT l_x = static_cast<bcFLOAT>(static_cast< double >(rand()) / RAND_MAX) * 2.0f - 1.0f;
			bcFLOAT l_y = static_cast<bcFLOAT>(static_cast< double >(rand()) / RAND_MAX) * 2.0f - 1.0f;
			bcFLOAT l_z = static_cast<bcFLOAT>(static_cast< double >(rand()) / RAND_MAX) * 2.0f - 1.0f;

			bc_vector3f l_random = bc_vector3f(l_x, l_y, l_z);
			l_random.normalize();

			return(l_random);
		}
	}
}