//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------

#include "Graphic/GraphicPCH.h"
#include "Graphic/Math/bcVector2f.h"
#include "Graphic/Math/bcVector3f.h"
#include "Graphic/Math/bcVector4f.h"

namespace black_cat
{
	namespace graphic
	{
		bc_vector4f::bc_vector4f() : x(0), y(0), z(0), w(0)
		{
		}
		
		bc_vector4f::bc_vector4f(float p_x, float p_y, float p_z, float p_w)
		{
			x = p_x;
			y = p_y;
			z = p_z;
			w = p_w;
		}
		
		bc_vector4f::bc_vector4f(const bc_vector3f& p_vector, float p_w)
		{
			x = p_vector.x;
			y = p_vector.y;
			z = p_vector.z;
			w = p_w;
		}
		
		bc_vector4f::bc_vector4f(const bc_vector4f& p_vector)
		{
			x = p_vector.x;
			y = p_vector.y;
			z = p_vector.z;
			w = p_vector.w;
		}
		
		bc_vector4f& bc_vector4f::operator= (const bc_vector4f& p_vector)
		{
			x = p_vector.x;
			y = p_vector.y;
			z = p_vector.z;
			w = p_vector.w;

			return *this;
		}
		
		void bc_vector4f::make_zero()
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
			w = 0.0f;
		}
		
		void bc_vector4f::normalize()
		{
			float l_inv_mag = (1.0f / magnitude());

			x *= l_inv_mag;
			y *= l_inv_mag;
			z *= l_inv_mag;
			w *= l_inv_mag;
		}
		
		float bc_vector4f::magnitude() const
		{
			float l_length = 0.0f;

			l_length += x * x;
			l_length += y * y;
			l_length += z * z;
			l_length += w * w;

			return(sqrt(l_length));
		}
		
		float bc_vector4f::dot(bc_vector4f& p_other) const
		{
			float l_ret = 0.0f;

			l_ret += x * p_other.x;
			l_ret += y * p_other.y;
			l_ret += z * p_other.z;
			l_ret += w * p_other.w;

			return l_ret;
		}
		
		float bc_vector4f::operator[] (int p_pos) const
		{
			if (p_pos == 0) return(x);
			if (p_pos == 1) return(y);
			if (p_pos == 2) return(z);
			return(w);
		}
		//----------------------------------------------------------------------------
		float& bc_vector4f::operator[] (int p_pos)
		{
			if (p_pos == 0) return(x);
			if (p_pos == 1) return(y);
			if (p_pos == 2) return(z);
			return(w);
		}
		//----------------------------------------------------------------------------
		bool bc_vector4f::operator== (const bc_vector4f& p_other) const
		{

			if ((x - p_other.x) * (x - p_other.x) > 0.01f)
				return false;
			if ((y - p_other.y) * (y - p_other.y) > 0.01f)
				return false;
			if ((z - p_other.z) * (z - p_other.z) > 0.01f)
				return false;
			if ((w - p_other.w) * (w - p_other.w) > 0.01f)
				return false;

			return true;
		}
		
		bool bc_vector4f::operator!= (const bc_vector4f& p_other) const
		{
			return(!(*this == p_other));
		}
		
		bc_vector4f bc_vector4f::operator+ (const bc_vector4f& p_other) const
		{
			bc_vector4f l_sum;

			l_sum.x = x + p_other.x;
			l_sum.y = y + p_other.y;
			l_sum.z = z + p_other.z;
			l_sum.w = w + p_other.w;

			return(l_sum);
		}
		
		bc_vector4f bc_vector4f::operator- (const bc_vector4f& p_other) const
		{
			bc_vector4f l_diff;

			l_diff.x = x - p_other.x;
			l_diff.y = y - p_other.y;
			l_diff.z = z - p_other.z;
			l_diff.w = w - p_other.w;

			return(l_diff);
		}
		
		bc_vector4f bc_vector4f::operator* (float p_scalar) const
		{
			bc_vector4f l_prod;

			l_prod.x = x * p_scalar;
			l_prod.y = y * p_scalar;
			l_prod.z = z * p_scalar;
			l_prod.w = w * p_scalar;

			return(l_prod);
		}
		
		bc_vector4f bc_vector4f::operator* (const bc_vector4f& p_other) const
		{
			bc_vector4f l_prod;

			l_prod.x = x * p_other.x;
			l_prod.y = y * p_other.y;
			l_prod.z = z * p_other.z;
			l_prod.w = w * p_other.w;

			return(l_prod);
		}
		
		bc_vector4f bc_vector4f::operator/ (float p_scalar) const
		{
			bc_vector4f l_quot;
			if (p_scalar != 0.0f)
			{
				float l_inv_scalar = 1.0f / p_scalar;
				l_quot.x = x * l_inv_scalar;
				l_quot.y = y * l_inv_scalar;
				l_quot.z = z * l_inv_scalar;
				l_quot.w = w * l_inv_scalar;
			}
			else
			{
				l_quot.make_zero();
			}

			return(l_quot);
		}
		
		bc_vector4f bc_vector4f::operator/ (const bc_vector4f& p_other) const
		{
			bc_vector4f l_quot;
			l_quot.x = p_other.x != 0.0f ? x / p_other.x : 0.0f;
			l_quot.y = p_other.y != 0.0f ? y / p_other.y : 0.0f;
			l_quot.z = p_other.z != 0.0f ? z / p_other.z : 0.0f;
			l_quot.w = p_other.w != 0.0f ? w / p_other.w : 0.0f;

			return(l_quot);
		}
		
		bc_vector4f bc_vector4f::operator- () const
		{
			bc_vector4f l_neg;

			l_neg.x = -x;
			l_neg.y = -y;
			l_neg.z = -z;
			l_neg.w = -w;

			return(l_neg);
		}
		
		bc_vector4f& bc_vector4f::operator+= (const bc_vector4f& p_other)
		{
			x += p_other.x;
			y += p_other.y;
			z += p_other.z;
			w += p_other.w;

			return(*this);
		}
		
		bc_vector4f& bc_vector4f::operator-= (const bc_vector4f& p_other)
		{
			x -= p_other.x;
			y -= p_other.y;
			z -= p_other.z;
			w -= p_other.w;

			return(*this);
		}
		
		bc_vector4f& bc_vector4f::operator*= (float p_scalar)
		{
			x *= p_scalar;
			y *= p_scalar;
			z *= p_scalar;
			w *= p_scalar;

			return(*this);
		}
		
		bc_vector4f& bc_vector4f::operator*= (const bc_vector4f& p_other)
		{
			x *= p_other.x;
			y *= p_other.y;
			z *= p_other.z;
			w *= p_other.w;

			return(*this);
		}
		
		bc_vector4f& bc_vector4f::operator/= (float p_scalar)
		{
			if (p_scalar != 0.0f)
			{
				float l_inv_scalar = 1.0f / p_scalar;
				x *= l_inv_scalar;
				y *= l_inv_scalar;
				z *= l_inv_scalar;
				w *= l_inv_scalar;
			}
			else
			{
				make_zero();
			}

			return(*this);
		}
		
		bc_vector4f& bc_vector4f::operator/= (const bc_vector4f& p_other)
		{
			x = p_other.x != 0.0f ? x / p_other.x : 0.0f;
			y = p_other.y != 0.0f ? y / p_other.y : 0.0f;
			z = p_other.z != 0.0f ? z / p_other.z : 0.0f;
			w = p_other.w != 0.0f ? w / p_other.w : 0.0f;

			return(*this);
		}
		
		void bc_vector4f::clamp()
		{
			if (x > 1.0f) x = 1.0f;
			if (x < 0.0f) x = 0.0f;

			if (y > 1.0f) y = 1.0f;
			if (y < 0.0f) y = 0.0f;

			if (z > 1.0f) z = 1.0f;
			if (z < 0.0f) z = 0.0f;

			if (w > 1.0f) w = 1.0f;
			if (w < 0.0f) w = 0.0f;
		}
		
		unsigned int bc_vector4f::to_ARGB()
		{
			unsigned int l_result = 0;

			clamp();

			l_result += static_cast<unsigned int>(255 * z);
			l_result += (static_cast<unsigned int>(255 * y) << 8);
			l_result += (static_cast<unsigned int>(255 * x) << 16);
			l_result += (static_cast<unsigned int>(255 * w) << 24);

			return(l_result);
		}
		
		unsigned int bc_vector4f::to_RGBA()
		{
			unsigned int l_result = 0;

			clamp();

			l_result += static_cast<unsigned int>(255 * w);
			l_result += (static_cast<unsigned int>(255 * z) << 8);
			l_result += (static_cast<unsigned int>(255 * y) << 16);
			l_result += (static_cast<unsigned int>(255 * x) << 24);

			return(l_result);
		}
		
		void bc_vector4f::from_ARGB(unsigned int color)
		{
			x = static_cast<float>((color & 0x00ff0000) >> 16) / (255.0f);	// red channel
			y = static_cast<float>((color & 0x0000ff00) >> 8) / (255.0f);	// green channel
			z = static_cast<float>((color & 0x000000ff)) / (255.0f);		// blue channel
			w = static_cast<float>((color & 0xff000000) >> 24) / (255.0f);	// alpha channel
		}
		
		bc_vector3f bc_vector4f::xyz() const
		{
			return(bc_vector3f(x, y, z));
		}
		
		bc_vector2f bc_vector4f::xy() const
		{
			return(bc_vector2f(x, y));
		}
	}
}