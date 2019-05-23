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
		template< typename T >
		class bc_vector2;
		template< typename T >
		class bc_vector3;

		template< typename T >
		class bc_vector4
		{
		public:
			bc_vector4();

			explicit bc_vector4(T p_number);

			bc_vector4(T p_x, T p_y, T p_z, T p_w);

			bc_vector4(const bc_vector3< T >& p_vector, T w);

			bc_vector4(const bc_vector4& p_vector);

			void clamp();

			bcFLOAT dot(bc_vector4& p_other) const;

			void normalize();

			bcFLOAT magnitude() const;

			void make_zero();

			bc_vector4& operator=(const bc_vector4& p_other);

			T operator[](bcUINT32 p_pos) const;

			T& operator[](bcUINT32 p_pos);

			bool operator==(const bc_vector4& p_other) const;

			bool operator!=(const bc_vector4& p_other) const;

			bc_vector4 operator+(const bc_vector4& p_other) const;

			bc_vector4 operator-(const bc_vector4& p_other) const;

			bc_vector4 operator*(T p_scalar) const;

			bc_vector4 operator*(const bc_vector4& p_other) const;

			bc_vector4 operator/(T p_scalar) const;

			bc_vector4 operator/(const bc_vector4& p_other) const;

			bc_vector4 operator-() const;

			bc_vector4& operator+=(const bc_vector4& p_other);

			bc_vector4& operator-=(const bc_vector4& p_other);

			bc_vector4& operator*=(T p_scalar);

			bc_vector4& operator*=(const bc_vector4& p_other);

			bc_vector4& operator/=(T p_scalar);

			bc_vector4& operator/=(const bc_vector4& p_other);

			bcUINT32 to_argb();

			bcUINT32 to_rgba();

			void from_argb(bcUINT32 color);

			bc_vector3< T > xyz() const;

			bc_vector2< T > xy() const;

			T x;
			T y;
			T z;
			T w;
		};

		using bc_vector4f = bc_vector4< bcFLOAT >;
		using bc_vector4i = bc_vector4< bcINT32 >;

		template< typename T >
		bc_vector4< T >::bc_vector4()
			: x(0),
			y(0),
			z(0),
			w(0)
		{
		}

		template< typename T >
		bc_vector4<T>::bc_vector4(T p_number)
			: x(p_number),
			y(p_number),
			z(p_number),
			w(p_number)
		{
		}

		template< typename T >
		bc_vector4< T >::bc_vector4(T p_x, T p_y, T p_z, T p_w)
		{
			x = p_x;
			y = p_y;
			z = p_z;
			w = p_w;
		}

		template< typename T >
		bc_vector4< T >::bc_vector4(const bc_vector3< T >& p_vector, T p_w)
		{
			x = p_vector.x;
			y = p_vector.y;
			z = p_vector.z;
			w = p_w;
		}

		template< typename T >
		bc_vector4< T >::bc_vector4(const bc_vector4< T >& p_vector)
		{
			x = p_vector.x;
			y = p_vector.y;
			z = p_vector.z;
			w = p_vector.w;
		}

		template< typename T >
		bc_vector4< T >& bc_vector4< T >::operator=(const bc_vector4& p_vector)
		{
			x = p_vector.x;
			y = p_vector.y;
			z = p_vector.z;
			w = p_vector.w;

			return *this;
		}

		template< typename T >
		void bc_vector4< T >::clamp()
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

		template< typename T >
		bcFLOAT bc_vector4< T >::dot(bc_vector4< T >& p_other) const
		{
			bcFLOAT l_ret = 0.0f;

			l_ret += x * p_other.x;
			l_ret += y * p_other.y;
			l_ret += z * p_other.z;
			l_ret += w * p_other.w;

			return l_ret;
		}

		template< typename T >
		void bc_vector4< T >::normalize()
		{
			bcFLOAT l_inv_mag = (1.0f / magnitude());

			x *= l_inv_mag;
			y *= l_inv_mag;
			z *= l_inv_mag;
			w *= l_inv_mag;
		}

		template< typename T >
		bcFLOAT bc_vector4< T >::magnitude() const
		{
			bcFLOAT l_length = 0.0f;

			l_length += x * x;
			l_length += y * y;
			l_length += z * z;
			l_length += w * w;

			return (sqrt(l_length));
		}

		template< typename T >
		void bc_vector4< T >::make_zero()
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
			w = 0.0f;
		}

		template< typename T >
		T bc_vector4< T >::operator[](bcUINT32 p_pos) const
		{
			if (p_pos == 0)
			{
				return x;
			}
			if (p_pos == 1)
			{
				return y;
			}
			if (p_pos == 2)
			{
				return z;
			}
			return w;
		}

		template< typename T >
		T& bc_vector4< T >::operator[](bcUINT32 p_pos)
		{
			if (p_pos == 0)
			{
				return x;
			}
			if (p_pos == 1)
			{
				return y;
			}
			if (p_pos == 2)
			{
				return z;
			}
			return w;
		}

		template< typename T >
		bool bc_vector4< T >::operator==(const bc_vector4< T >& p_other) const
		{
			if ((x - p_other.x) * (x - p_other.x) > 0.01f)
			{
				return false;
			}
			if ((y - p_other.y) * (y - p_other.y) > 0.01f)
			{
				return false;
			}
			if ((z - p_other.z) * (z - p_other.z) > 0.01f)
			{
				return false;
			}
			if ((w - p_other.w) * (w - p_other.w) > 0.01f)
			{
				return false;
			}

			return true;
		}

		template< typename T >
		bool bc_vector4< T >::operator!=(const bc_vector4< T >& p_other) const
		{
			return (!(*this == p_other));
		}

		template< typename T >
		bc_vector4< T > bc_vector4< T >::operator+(const bc_vector4< T >& p_other) const
		{
			bc_vector4< T > l_sum;

			l_sum.x = x + p_other.x;
			l_sum.y = y + p_other.y;
			l_sum.z = z + p_other.z;
			l_sum.w = w + p_other.w;

			return l_sum;
		}

		template< typename T >
		bc_vector4< T > bc_vector4< T >::operator-(const bc_vector4< T >& p_other) const
		{
			bc_vector4< T > l_diff;

			l_diff.x = x - p_other.x;
			l_diff.y = y - p_other.y;
			l_diff.z = z - p_other.z;
			l_diff.w = w - p_other.w;

			return l_diff;
		}

		template< typename T >
		bc_vector4< T > bc_vector4< T >::operator*(T p_scalar) const
		{
			bc_vector4< T > l_prod;

			l_prod.x = x * p_scalar;
			l_prod.y = y * p_scalar;
			l_prod.z = z * p_scalar;
			l_prod.w = w * p_scalar;

			return (l_prod);
		}

		template< typename T >
		bc_vector4< T > bc_vector4< T >::operator*(const bc_vector4< T >& p_other) const
		{
			bc_vector4< T > l_prod;

			l_prod.x = x * p_other.x;
			l_prod.y = y * p_other.y;
			l_prod.z = z * p_other.z;
			l_prod.w = w * p_other.w;

			return (l_prod);
		}

		template< typename T >
		bc_vector4< T > bc_vector4< T >::operator/(T p_scalar) const
		{
			bc_vector4< T > l_quot;
			if (p_scalar != 0.0f)
			{
				bcFLOAT l_inv_scalar = 1.0f / p_scalar;
				l_quot.x = x * l_inv_scalar;
				l_quot.y = y * l_inv_scalar;
				l_quot.z = z * l_inv_scalar;
				l_quot.w = w * l_inv_scalar;
			}
			else
			{
				l_quot.make_zero();
			}

			return l_quot;
		}

		template< typename T >
		bc_vector4< T > bc_vector4< T >::operator/(const bc_vector4< T >& p_other) const
		{
			bc_vector4< T > l_quot;
			l_quot.x = p_other.x != 0.0f ? x / p_other.x : 0.0f;
			l_quot.y = p_other.y != 0.0f ? y / p_other.y : 0.0f;
			l_quot.z = p_other.z != 0.0f ? z / p_other.z : 0.0f;
			l_quot.w = p_other.w != 0.0f ? w / p_other.w : 0.0f;

			return l_quot;
		}

		template< typename T >
		bc_vector4< T > bc_vector4< T >::operator-() const
		{
			bc_vector4< T > l_neg;

			l_neg.x = -x;
			l_neg.y = -y;
			l_neg.z = -z;
			l_neg.w = -w;

			return l_neg;
		}

		template< typename T >
		bc_vector4< T >& bc_vector4< T >::operator+=(const bc_vector4& p_other)
		{
			x += p_other.x;
			y += p_other.y;
			z += p_other.z;
			w += p_other.w;

			return *this;
		}

		template< typename T >
		bc_vector4< T >& bc_vector4< T >::operator-=(const bc_vector4& p_other)
		{
			x -= p_other.x;
			y -= p_other.y;
			z -= p_other.z;
			w -= p_other.w;

			return *this;
		}

		template< typename T >
		bc_vector4< T >& bc_vector4< T >::operator*=(T p_scalar)
		{
			x *= p_scalar;
			y *= p_scalar;
			z *= p_scalar;
			w *= p_scalar;

			return *this;
		}

		template< typename T >
		bc_vector4< T >& bc_vector4< T >::operator*=(const bc_vector4& p_other)
		{
			x *= p_other.x;
			y *= p_other.y;
			z *= p_other.z;
			w *= p_other.w;

			return *this;
		}

		template< typename T >
		bc_vector4< T >& bc_vector4< T >::operator/=(T p_scalar)
		{
			if (p_scalar != 0.0f)
			{
				bcFLOAT l_inv_scalar = 1.0f / p_scalar;
				x *= l_inv_scalar;
				y *= l_inv_scalar;
				z *= l_inv_scalar;
				w *= l_inv_scalar;
			}
			else
			{
				make_zero();
			}

			return *this;
		}

		template< typename T >
		bc_vector4< T >& bc_vector4< T >::operator/=(const bc_vector4& p_other)
		{
			x = p_other.x != 0.0f ? x / p_other.x : 0.0f;
			y = p_other.y != 0.0f ? y / p_other.y : 0.0f;
			z = p_other.z != 0.0f ? z / p_other.z : 0.0f;
			w = p_other.w != 0.0f ? w / p_other.w : 0.0f;

			return *this;
		}

		template< typename T >
		bcUINT32 bc_vector4< T >::to_argb()
		{
			bcUINT32 l_result = 0;
			bc_vector4< T > l_temp = *this;

			l_temp.clamp();

			l_result += static_cast< bcUINT32 >(255 * l_temp.z);
			l_result += (static_cast< bcUINT32 >(255 * l_temp.y) << 8);
			l_result += (static_cast< bcUINT32 >(255 * l_temp.x) << 16);
			l_result += (static_cast< bcUINT32 >(255 * l_temp.w) << 24);

			return l_result;
		}

		template< typename T >
		bcUINT32 bc_vector4< T >::to_rgba()
		{
			bcUINT32 l_result = 0;
			bc_vector4< T > l_temp = *this;

			l_temp.clamp();

			l_result += static_cast< bcUINT32 >(255 * l_temp.w);
			l_result += (static_cast< bcUINT32 >(255 * l_temp.z) << 8);
			l_result += (static_cast< bcUINT32 >(255 * l_temp.y) << 16);
			l_result += (static_cast< bcUINT32 >(255 * l_temp.x) << 24);

			return l_result;
		}

		template< typename T >
		void bc_vector4< T >::from_argb(bcUINT32 color)
		{
			x = static_cast< float >((color & 0x00ff0000) >> 16) / 255.0f; // red channel
			y = static_cast< float >((color & 0x0000ff00) >> 8) / 255.0f; // green channel
			z = static_cast< float >((color & 0x000000ff)) / 255.0f; // blue channel
			w = static_cast< float >((color & 0xff000000) >> 24) / 255.0f; // alpha channel
		}

		template< typename T >
		bc_vector3< T > bc_vector4< T >::xyz() const
		{
			return bc_vector3f(x, y, z);
		}

		template< typename T >
		bc_vector2< T > bc_vector4< T >::xy() const
		{
			return bc_vector2f(x, y);
		}
	}
}
