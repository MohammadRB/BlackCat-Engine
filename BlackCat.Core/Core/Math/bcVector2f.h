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
		class bc_vector2
		{
		public:
			bc_vector2();

			bc_vector2(T x, T y);

			bc_vector2(const bc_vector2& p_other);

			void clamp();

			void normalize();

			bcFLOAT magnitude();

			void make_zero();

			bc_vector2& operator=(const bc_vector2& p_other);

			T operator[](int p_pos) const;

			T& operator[](int p_pos);

			bool operator==(const bc_vector2& p_other) const;

			bool operator!=(const bc_vector2& p_other) const;

			bc_vector2 operator+(const bc_vector2& p_other) const;

			bc_vector2 operator-(const bc_vector2& p_other) const;

			bc_vector2 operator*(const bc_vector2& p_other) const;

			bc_vector2 operator*(T p_scalar) const;

			bc_vector2 operator/(T p_scalar) const;

			bc_vector2 operator-() const;

			bc_vector2& operator+=(const bc_vector2& p_other);

			bc_vector2& operator-=(const bc_vector2& p_other);

			bc_vector2& operator*=(T p_scalar);

			bc_vector2& operator/=(T p_scalar);

		public:
			T x;
			T y;
		};

		using bc_vector2f = bc_vector2< bcFLOAT >;
		using bc_vector2i = bc_vector2< bcINT32 >;

		template< typename T >
		bc_vector2< T >::bc_vector2()
			: x(0),
			y(0)
		{
		}

		template< typename T >
		bc_vector2< T >::bc_vector2(T p_x, T p_y)
		{
			x = p_x;
			y = p_y;
		}

		template< typename T >
		bc_vector2< T >::bc_vector2(const bc_vector2< T >& p_vector)
		{
			x = p_vector.x;
			y = p_vector.y;
		}

		template< typename T >
		bc_vector2< T >& bc_vector2< T >::operator=(const bc_vector2& p_vector)
		{
			x = p_vector.x;
			y = p_vector.y;

			return (*this);
		}

		template< typename T >
		void bc_vector2< T >::clamp()
		{
			if (x > 1.0f) x = 1.0f;
			if (x < 0.0f) x = 0.0f;

			if (y > 1.0f) y = 1.0f;
			if (y < 0.0f) y = 0.0f;
		}

		template< typename T >
		void bc_vector2< T >::normalize()
		{
			T l_inv_mag = (1.0f / magnitude());

			x *= l_inv_mag;
			y *= l_inv_mag;
		}

		template< typename T >
		bcFLOAT bc_vector2< T >::magnitude()
		{
			bcFLOAT l_length = 0.0f;

			l_length += x * x;
			l_length += y * y;

			return std::sqrtf(l_length);
		}

		template< typename T >
		void bc_vector2< T >::make_zero()
		{
			x = 0.0f;
			y = 0.0f;
		}

		template< typename T >
		T bc_vector2< T >::operator[](int p_pos) const
		{
			if (p_pos == 0)
			{
				return x;
			}

			return y;
		}

		template< typename T >
		T& bc_vector2< T >::operator[](int p_pos)
		{
			if (p_pos == 0)
			{
				return x;
			}

			return y;
		}

		template< typename T >
		bool bc_vector2< T >::operator==(const bc_vector2< T >& p_vector) const
		{
			if ((x - p_vector.x) * (x - p_vector.x) > 0.01f)
			{
				return false;
			}
			if ((y - p_vector.y) * (y - p_vector.y) > 0.01f)
			{
				return false;
			}

			return true;
		}

		template< typename T >
		bool bc_vector2< T >::operator!=(const bc_vector2< T >& p_vector) const
		{
			return !(*this == p_vector);
		}

		template< typename T >
		bc_vector2< T > bc_vector2< T >::operator+(const bc_vector2< T >& p_vector) const
		{
			bc_vector2< T > l_sum;

			l_sum.x = x + p_vector.x;
			l_sum.y = y + p_vector.y;

			return l_sum;
		}

		template< typename T >
		bc_vector2< T > bc_vector2< T >::operator-(const bc_vector2< T >& p_vector) const
		{
			bc_vector2< T > l_diff;

			l_diff.x = x - p_vector.x;
			l_diff.y = y - p_vector.y;

			return l_diff;
		}

		template< typename T >
		bc_vector2< T > bc_vector2< T >::operator*(const bc_vector2< T >& p_vector) const
		{
			bc_vector2< T > l_prod;

			l_prod.x = x * p_vector.x;
			l_prod.y = y * p_vector.y;

			return l_prod;
		}

		template< typename T >
		bc_vector2< T > bc_vector2< T >::operator*(T p_scalar) const
		{
			bc_vector2< T > l_prod;

			l_prod.x = x * p_scalar;
			l_prod.y = y * p_scalar;

			return l_prod;
		}

		template< typename T >
		bc_vector2< T > bc_vector2< T >::operator/(T p_scalar) const
		{
			bc_vector2< T > l_quot;
			if (p_scalar != 0.0f)
			{
				bcFLOAT l_inv_scalar = 1.0f / p_scalar;
				l_quot.x = x * l_inv_scalar;
				l_quot.y = y * l_inv_scalar;
			}
			else
			{
				l_quot.make_zero();
			}

			return l_quot;
		}

		template< typename T >
		bc_vector2< T > bc_vector2< T >::operator-() const
		{
			bc_vector2< T > l_neg;

			l_neg.x = -x;
			l_neg.y = -y;

			return l_neg;
		}

		template< typename T >
		bc_vector2< T >& bc_vector2< T >::operator+=(const bc_vector2& p_vector)
		{
			x += p_vector.x;
			y += p_vector.y;

			return *this;
		}

		template< typename T >
		bc_vector2< T >& bc_vector2< T >::operator-=(const bc_vector2& p_vector)
		{
			x -= p_vector.x;
			y -= p_vector.y;

			return *this;
		}

		template< typename T >
		bc_vector2< T >& bc_vector2< T >::operator*=(T p_scalar)
		{
			x *= p_scalar;
			y *= p_scalar;

			return *this;
		}

		template< typename T >
		bc_vector2< T >& bc_vector2< T >::operator/=(T p_scalar)
		{
			if (p_scalar != 0.0f)
			{
				bcFLOAT l_inv_scalar = 1.0f / p_scalar;
				x *= l_inv_scalar;
				y *= l_inv_scalar;
			}
			else
			{
				make_zero();
			}

			return *this;
		}
	}
}