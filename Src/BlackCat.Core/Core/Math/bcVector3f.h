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
		class bc_vector3
		{
		public:
			bc_vector3();

			bc_vector3(T p_x, T p_y, T p_z);

			bc_vector3(const bc_vector3& p_other);

			void clamp();

			bc_vector3 cross(const bc_vector3& p_vector) const;

			bcFLOAT dot(const bc_vector3& p_vector) const;

			bcFLOAT magnitude();

			void normalize();

			bc_vector3 perpendicular();

			void make_zero();

			bc_vector3& operator=(const bc_vector3& p_other);

			T operator[](bcUINT32 p_pos) const;

			T& operator[](bcUINT32 p_pos);

			bool operator==(const bc_vector3& p_other) const;

			bool operator!=(const bc_vector3& p_other) const;

			bc_vector3 operator+(const bc_vector3& p_other) const;

			bc_vector3 operator-(const bc_vector3& p_other) const;

			bc_vector3 operator*(const bc_vector3& p_other) const;

			bc_vector3 operator*(T p_scalar) const;

			bc_vector3 operator/(const bc_vector3& p_other) const;

			bc_vector3 operator/(T p_scalar) const;

			bc_vector3 operator-() const;

			bc_vector3& operator+=(const bc_vector3& p_other);

			bc_vector3& operator-=(const bc_vector3& p_other);

			bc_vector3& operator*=(const bc_vector3& p_other);

			bc_vector3& operator*=(T p_scalar);

			bc_vector3& operator/=(const bc_vector3& p_other);

			bc_vector3& operator/=(T p_scalar);

			static bc_vector3 clamp(const bc_vector3& p_first);

			static bc_vector3 cross(const bc_vector3& p_first, const bc_vector3& p_second);

			static bcFLOAT dot(const bc_vector3& p_first, const bc_vector3& p_second);

			static bcFLOAT length_sq(const bc_vector3& p_vector);

			static bc_vector3 normalize(const bc_vector3& p_vector);

			static bc_vector3 perpendicular(const bc_vector3& p_vector);

			static bc_vector3 random();

			static const bc_vector3& up();

			static const bc_vector3& right();

			static const bc_vector3& forward();

		public:
			T x;
			T y;
			T z;
		};

		using bc_vector3f = bc_vector3<bcFLOAT>;
		using bc_vector3i = bc_vector3<bcINT32>;

		template< typename T >
		bc_vector3<T>::bc_vector3()
			: x(0),
			y(0),
			z(0)
		{
		}

		template< typename T >
		bc_vector3<T>::bc_vector3(T p_x, T p_y, T p_z)
		{
			x = p_x;
			y = p_y;
			z = p_z;
		}

		template< typename T >
		bc_vector3<T>::bc_vector3(const bc_vector3<T>& p_other)
		{
			x = p_other.x;
			y = p_other.y;
			z = p_other.z;
		}

		template< typename T >
		void bc_vector3<T>::clamp()
		{
			if (x > 1.0f) x = 1.0f;
			if (x < 0.0f) x = 0.0f;

			if (y > 1.0f) y = 1.0f;
			if (y < 0.0f) y = 0.0f;

			if (z > 1.0f) z = 1.0f;
			if (z < 0.0f) z = 0.0f;
		}

		template< typename T >
		bcFLOAT bc_vector3<T>::dot(const bc_vector3<T>& p_other) const
		{
			bcFLOAT l_ret = 0.0f;

			l_ret = x * p_other.x;
			l_ret += y * p_other.y;
			l_ret += z * p_other.z;

			return l_ret;
		}

		template< typename T >
		bc_vector3<T> bc_vector3<T>::cross(const bc_vector3<T>& p_other) const
		{
			bc_vector3<T> l_ret;

			l_ret.x = y * p_other.z - z * p_other.y;
			l_ret.y = z * p_other.x - x * p_other.z;
			l_ret.z = x * p_other.y - y * p_other.x;

			return l_ret;
		}

		template< typename T >
		bcFLOAT bc_vector3<T>::magnitude()
		{
			bcFLOAT l_length = 0.0f;

			l_length += x * x;
			l_length += y * y;
			l_length += z * z;

			return std::sqrt(l_length);
		}

		template< typename T >
		void bc_vector3<T>::normalize()
		{
			bcFLOAT l_mag = magnitude();
			if (0.0f == l_mag)
			{
				l_mag = 0.0001f;
			}

			bcFLOAT l_inv_mag = (1.0f / l_mag);

			x *= l_inv_mag;
			y *= l_inv_mag;
			z *= l_inv_mag;
		}

		template< typename T >
		bc_vector3<T> bc_vector3<T>::perpendicular()
		{
			auto l_x_abs = std::fabs(x);
			auto l_y_abs = std::fabs(y);
			auto l_z_abs = std::fabs(z);
			auto l_min_val = std::min(std::min(l_x_abs, l_y_abs), l_z_abs);

			if (l_x_abs == l_min_val)
			{
				return cross(bc_vector3<T>(1.0f, 0.0f, 0.0f));
			}
			if (l_y_abs == l_min_val)
			{
				return cross(bc_vector3<T>(0.0f, 1.0f, 0.0f));
			}
			return cross(bc_vector3<T>(0.0f, 0.0f, 1.0f));
		}

		template< typename T >
		void bc_vector3<T>::make_zero()
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
		}

		template< typename T >
		bc_vector3<T>& bc_vector3<T>::operator=(const bc_vector3& p_other)
		{
			x = p_other.x;
			y = p_other.y;
			z = p_other.z;

			return *this;
		}

		template< typename T >
		T bc_vector3<T>::operator[](bcUINT32 p_pos) const
		{
			if (p_pos == 0)
			{
				return x;
			}
			if (p_pos == 1)
			{
				return y;
			}
			return z;
		}

		template< typename T >
		T& bc_vector3<T>::operator[](bcUINT32 p_pos)
		{
			if (p_pos == 0)
			{
				return x;
			}
			if (p_pos == 1)
			{
				return y;
			}
			return z;
		}

		template< typename T >
		bool bc_vector3<T>::operator==(const bc_vector3<T>& p_other) const
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

			return true;
		}

		template< typename T >
		bool bc_vector3<T>::operator!=(const bc_vector3<T>& p_other) const
		{
			return (!(*this == p_other));
		}

		template< typename T >
		bc_vector3<T> bc_vector3<T>::operator+(const bc_vector3<T>& p_other) const
		{
			bc_vector3<T> l_sum;

			l_sum.x = x + p_other.x;
			l_sum.y = y + p_other.y;
			l_sum.z = z + p_other.z;

			return l_sum;
		}

		template< typename T >
		bc_vector3<T> bc_vector3<T>::operator-(const bc_vector3<T>& p_other) const
		{
			bc_vector3<T> l_diff;

			l_diff.x = x - p_other.x;
			l_diff.y = y - p_other.y;
			l_diff.z = z - p_other.z;

			return l_diff;
		}

		template< typename T >
		bc_vector3<T> bc_vector3<T>::operator*(T p_scalar) const
		{
			bc_vector3<T> l_prod;

			l_prod.x = x * p_scalar;
			l_prod.y = y * p_scalar;
			l_prod.z = z * p_scalar;

			return l_prod;
		}

		template< typename T >
		bc_vector3<T> bc_vector3<T>::operator*(const bc_vector3<T>& p_vector) const
		{
			bc_vector3<T> l_prod;

			l_prod.x = x * p_vector.x;
			l_prod.y = y * p_vector.y;
			l_prod.z = z * p_vector.z;

			return l_prod;
		}

		template< typename T >
		bc_vector3<T> bc_vector3<T>::operator/(T p_scalar) const
		{
			bc_vector3<T> l_quot;
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

			return l_quot;
		}

		template< typename T >
		bc_vector3<T> bc_vector3<T>::operator/(const bc_vector3<T>& p_other) const
		{
			bc_vector3<T> l_quot;
			l_quot.x = p_other.x != 0.0f ? x / p_other.x : 0.0f;
			l_quot.y = p_other.y != 0.0f ? y / p_other.y : 0.0f;
			l_quot.z = p_other.z != 0.0f ? z / p_other.z : 0.0f;

			return l_quot;
		}

		template< typename T >
		bc_vector3<T> bc_vector3<T>::operator-() const
		{
			bc_vector3<T> l_neg;

			l_neg.x = -x;
			l_neg.y = -y;
			l_neg.z = -z;

			return l_neg;
		}

		template< typename T >
		bc_vector3<T>& bc_vector3<T>::operator+=(const bc_vector3& p_other)
		{
			x += p_other.x;
			y += p_other.y;
			z += p_other.z;

			return *this;
		}

		template< typename T >
		bc_vector3<T>& bc_vector3<T>::operator-=(const bc_vector3& p_other)
		{
			x -= p_other.x;
			y -= p_other.y;
			z -= p_other.z;

			return *this;
		}

		template< typename T >
		bc_vector3<T>& bc_vector3<T>::operator*=(T p_scalar)
		{
			x *= p_scalar;
			y *= p_scalar;
			z *= p_scalar;

			return *this;
		}

		template< typename T >
		bc_vector3<T>& bc_vector3<T>::operator*=(const bc_vector3& p_other)
		{
			x *= p_other.x;
			y *= p_other.y;
			z *= p_other.z;

			return *this;
		}

		template< typename T >
		bc_vector3<T>& bc_vector3<T>::operator/=(T p_scalar)
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

			return *this;
		}

		template< typename T >
		bc_vector3<T>& bc_vector3<T>::operator/=(const bc_vector3& p_other)
		{
			x = p_other.x != 0.0f ? x / p_other.x : 0.0f;
			y = p_other.y != 0.0f ? y / p_other.y : 0.0f;
			z = p_other.z != 0.0f ? z / p_other.z : 0.0f;

			return *this;
		}

		template< typename T >
		bc_vector3<T> bc_vector3<T>::clamp(const bc_vector3<T>& p_vector)
		{
			bc_vector3<T> vec = p_vector;
			vec.clamp();
			return vec;
		}

		template< typename T >
		bc_vector3<T> bc_vector3<T>::cross(const bc_vector3<T>& p_first, const bc_vector3<T>& p_second)
		{
			return p_first.cross(p_second);
		}

		template< typename T >
		bcFLOAT bc_vector3<T>::dot(const bc_vector3<T>& p_first, const bc_vector3<T>& p_second)
		{
			return p_first.dot(p_second);
		}

		template< typename T >
		bcFLOAT bc_vector3<T>::length_sq(const bc_vector3<T>& p_vector)
		{
			return dot(p_vector, p_vector);
		}

		template< typename T >
		bc_vector3<T> bc_vector3<T>::normalize(const bc_vector3<T>& p_vector)
		{
			bc_vector3<T> l_vec = p_vector;
			l_vec.normalize();
			return l_vec;
		}

		template< typename T >
		bc_vector3<T> bc_vector3<T>::perpendicular(const bc_vector3<T>& p_vector)
		{
			bc_vector3<T> vec = p_vector;
			return vec.perpendicular();
		}

		template< typename T >
		bc_vector3<T> bc_vector3<T>::random()
		{
			bcFLOAT l_x = static_cast< bcFLOAT >(static_cast< double >(rand()) / RAND_MAX) * 2.0f - 1.0f;
			bcFLOAT l_y = static_cast< bcFLOAT >(static_cast< double >(rand()) / RAND_MAX) * 2.0f - 1.0f;
			bcFLOAT l_z = static_cast< bcFLOAT >(static_cast< double >(rand()) / RAND_MAX) * 2.0f - 1.0f;

			bc_vector3<T> l_random = bc_vector3(l_x, l_y, l_z);
			l_random.normalize();

			return (l_random);
		}

		template< typename T >
		const bc_vector3<T>& bc_vector3<T>::up()
		{
			static bc_vector3<T> s_up{ 0,1,0 };
			return s_up;
		}

		template< typename T >
		const bc_vector3<T>& bc_vector3<T>::right()
		{
			static bc_vector3<T> s_right{ 1,0,0 };
			return s_right;
		}

		template< typename T >
		const bc_vector3<T>& bc_vector3<T>::forward()
		{
			static bc_vector3<T> s_forward{ 0,0,1 };
			return s_forward;
		}
	}
}