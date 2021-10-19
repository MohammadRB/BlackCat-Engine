#pragma once

#include "CorePlatform/bcType.h"
#include "Core/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_logger_output_stream;
		
		template<typename T>
		class bc_vector2;
		
		template<typename T>
		class bc_vector3;

		template<typename T>
		class bc_vector4
		{
		public:
			bc_vector4() noexcept;

			explicit bc_vector4(T p_number) noexcept;

			bc_vector4(T p_x, T p_y, T p_z, T p_w) noexcept;

			bc_vector4(const bc_vector3<T>& p_vector, T p_w) noexcept;

			bc_vector4(const bc_vector4& p_vector) noexcept;

			bc_vector4& operator=(const bc_vector4& p_other) noexcept;

			void clamp() noexcept;

			bcFLOAT dot(bc_vector4& p_other) const noexcept;

			void normalize() noexcept;

			bcFLOAT magnitude() const noexcept;

			void make_zero() noexcept;

			T operator[](bcUINT32 p_pos) const noexcept;

			T& operator[](bcUINT32 p_pos) noexcept;

			bool operator==(const bc_vector4& p_other) const noexcept;

			bool operator!=(const bc_vector4& p_other) const noexcept;

			bc_vector4 operator+(const bc_vector4& p_other) const noexcept;

			bc_vector4 operator-(const bc_vector4& p_other) const noexcept;

			bc_vector4 operator*(T p_scalar) const noexcept;

			bc_vector4 operator*(const bc_vector4& p_other) const noexcept;

			bc_vector4 operator/(T p_scalar) const noexcept;

			bc_vector4 operator/(const bc_vector4& p_other) const noexcept;

			bc_vector4 operator-() const noexcept;

			bc_vector4& operator+=(const bc_vector4& p_other) noexcept;

			bc_vector4& operator-=(const bc_vector4& p_other) noexcept;

			bc_vector4& operator*=(T p_scalar) noexcept;

			bc_vector4& operator*=(const bc_vector4& p_other) noexcept;

			bc_vector4& operator/=(T p_scalar) noexcept;

			bc_vector4& operator/=(const bc_vector4& p_other) noexcept;

			bcUINT32 to_argb() const noexcept;

			bcUINT32 to_rgba() const noexcept;

			void from_argb(bcUINT32 color) noexcept;

			bc_vector3<T> xyz() const noexcept;

			bc_vector2<T> xy() const noexcept;

			T x;
			T y;
			T z;
			T w;
		};

		using bc_vector4f = bc_vector4<bcFLOAT>;
		using bc_vector4i = bc_vector4<bcINT32>;

		BC_CORE_DLL bc_logger_output_stream& operator<<(bc_logger_output_stream& p_stream, const bc_vector4f& p_vector);

		BC_CORE_DLL bc_logger_output_stream& operator<<(bc_logger_output_stream& p_stream, const bc_vector4i& p_vector);

		template<typename T>
		bc_vector4<T>::bc_vector4() noexcept
			: x(0),
			y(0),
			z(0),
			w(0)
		{
		}

		template<typename T>
		bc_vector4<T>::bc_vector4(T p_number) noexcept
			: x(p_number),
			y(p_number),
			z(p_number),
			w(p_number)
		{
		}

		template<typename T>
		bc_vector4<T>::bc_vector4(T p_x, T p_y, T p_z, T p_w) noexcept
		{
			x = p_x;
			y = p_y;
			z = p_z;
			w = p_w;
		}

		template<typename T>
		bc_vector4<T>::bc_vector4(const bc_vector3<T>& p_vector, T p_w) noexcept
		{
			x = p_vector.x;
			y = p_vector.y;
			z = p_vector.z;
			w = p_w;
		}

		template<typename T>
		bc_vector4<T>::bc_vector4(const bc_vector4<T>& p_vector) noexcept
		{
			x = p_vector.x;
			y = p_vector.y;
			z = p_vector.z;
			w = p_vector.w;
		}

		template<typename T>
		bc_vector4<T>& bc_vector4<T>::operator=(const bc_vector4& p_other) noexcept
		{
			x = p_other.x;
			y = p_other.y;
			z = p_other.z;
			w = p_other.w;

			return *this;
		}

		template<typename T>
		void bc_vector4<T>::clamp() noexcept
		{
			if (x> 1.0f) x = 1.0f;
			if (x <0.0f) x = 0.0f;

			if (y> 1.0f) y = 1.0f;
			if (y <0.0f) y = 0.0f;

			if (z> 1.0f) z = 1.0f;
			if (z <0.0f) z = 0.0f;

			if (w> 1.0f) w = 1.0f;
			if (w <0.0f) w = 0.0f;
		}

		template<typename T>
		bcFLOAT bc_vector4<T>::dot(bc_vector4<T>& p_other) const noexcept
		{
			bcFLOAT l_ret = 0.0f;

			l_ret += x * p_other.x;
			l_ret += y * p_other.y;
			l_ret += z * p_other.z;
			l_ret += w * p_other.w;

			return l_ret;
		}

		template<typename T>
		void bc_vector4<T>::normalize() noexcept
		{
			bcFLOAT l_inv_mag = (1.0f / magnitude());

			x *= l_inv_mag;
			y *= l_inv_mag;
			z *= l_inv_mag;
			w *= l_inv_mag;
		}

		template<typename T>
		bcFLOAT bc_vector4<T>::magnitude() const noexcept
		{
			bcFLOAT l_length = 0.0f;

			l_length += x * x;
			l_length += y * y;
			l_length += z * z;
			l_length += w * w;

			return (sqrt(l_length));
		}

		template<typename T>
		void bc_vector4<T>::make_zero() noexcept
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
			w = 0.0f;
		}

		template<typename T>
		T bc_vector4<T>::operator[](bcUINT32 p_pos) const noexcept
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

		template<typename T>
		T& bc_vector4<T>::operator[](bcUINT32 p_pos) noexcept
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

		template<typename T>
		bool bc_vector4<T>::operator==(const bc_vector4<T>& p_other) const noexcept
		{
			if ((x - p_other.x) * (x - p_other.x)> 0.01f)
			{
				return false;
			}
			if ((y - p_other.y) * (y - p_other.y)> 0.01f)
			{
				return false;
			}
			if ((z - p_other.z) * (z - p_other.z)> 0.01f)
			{
				return false;
			}
			if ((w - p_other.w) * (w - p_other.w)> 0.01f)
			{
				return false;
			}

			return true;
		}

		template<typename T>
		bool bc_vector4<T>::operator!=(const bc_vector4<T>& p_other) const noexcept
		{
			return (!(*this == p_other));
		}

		template<typename T>
		bc_vector4<T> bc_vector4<T>::operator+(const bc_vector4<T>& p_other) const noexcept
		{
			bc_vector4<T> l_sum;

			l_sum.x = x + p_other.x;
			l_sum.y = y + p_other.y;
			l_sum.z = z + p_other.z;
			l_sum.w = w + p_other.w;

			return l_sum;
		}

		template<typename T>
		bc_vector4<T> bc_vector4<T>::operator-(const bc_vector4<T>& p_other) const noexcept
		{
			bc_vector4<T> l_diff;

			l_diff.x = x - p_other.x;
			l_diff.y = y - p_other.y;
			l_diff.z = z - p_other.z;
			l_diff.w = w - p_other.w;

			return l_diff;
		}

		template<typename T>
		bc_vector4<T> bc_vector4<T>::operator*(T p_scalar) const noexcept
		{
			bc_vector4<T> l_prod;

			l_prod.x = x * p_scalar;
			l_prod.y = y * p_scalar;
			l_prod.z = z * p_scalar;
			l_prod.w = w * p_scalar;

			return (l_prod);
		}

		template<typename T>
		bc_vector4<T> bc_vector4<T>::operator*(const bc_vector4<T>& p_other) const noexcept
		{
			bc_vector4<T> l_prod;

			l_prod.x = x * p_other.x;
			l_prod.y = y * p_other.y;
			l_prod.z = z * p_other.z;
			l_prod.w = w * p_other.w;

			return (l_prod);
		}

		template<typename T>
		bc_vector4<T> bc_vector4<T>::operator/(T p_scalar) const noexcept
		{
			bc_vector4<T> l_quot;
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

		template<typename T>
		bc_vector4<T> bc_vector4<T>::operator/(const bc_vector4<T>& p_other) const noexcept
		{
			bc_vector4<T> l_quot;
			l_quot.x = p_other.x != 0.0f ? x / p_other.x : 0.0f;
			l_quot.y = p_other.y != 0.0f ? y / p_other.y : 0.0f;
			l_quot.z = p_other.z != 0.0f ? z / p_other.z : 0.0f;
			l_quot.w = p_other.w != 0.0f ? w / p_other.w : 0.0f;

			return l_quot;
		}

		template<typename T>
		bc_vector4<T> bc_vector4<T>::operator-() const noexcept
		{
			bc_vector4<T> l_neg;

			l_neg.x = -x;
			l_neg.y = -y;
			l_neg.z = -z;
			l_neg.w = -w;

			return l_neg;
		}

		template<typename T>
		bc_vector4<T>& bc_vector4<T>::operator+=(const bc_vector4& p_other) noexcept
		{
			x += p_other.x;
			y += p_other.y;
			z += p_other.z;
			w += p_other.w;

			return *this;
		}

		template<typename T>
		bc_vector4<T>& bc_vector4<T>::operator-=(const bc_vector4& p_other) noexcept
		{
			x -= p_other.x;
			y -= p_other.y;
			z -= p_other.z;
			w -= p_other.w;

			return *this;
		}

		template<typename T>
		bc_vector4<T>& bc_vector4<T>::operator*=(T p_scalar) noexcept
		{
			x *= p_scalar;
			y *= p_scalar;
			z *= p_scalar;
			w *= p_scalar;

			return *this;
		}

		template<typename T>
		bc_vector4<T>& bc_vector4<T>::operator*=(const bc_vector4& p_other) noexcept
		{
			x *= p_other.x;
			y *= p_other.y;
			z *= p_other.z;
			w *= p_other.w;

			return *this;
		}

		template<typename T>
		bc_vector4<T>& bc_vector4<T>::operator/=(T p_scalar) noexcept
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

		template<typename T>
		bc_vector4<T>& bc_vector4<T>::operator/=(const bc_vector4& p_other) noexcept
		{
			x = p_other.x != 0.0f ? x / p_other.x : 0.0f;
			y = p_other.y != 0.0f ? y / p_other.y : 0.0f;
			z = p_other.z != 0.0f ? z / p_other.z : 0.0f;
			w = p_other.w != 0.0f ? w / p_other.w : 0.0f;

			return *this;
		}

		template<typename T>
		bcUINT32 bc_vector4<T>::to_argb() const noexcept
		{
			bcUINT32 l_result = 0;
			bc_vector4<T> l_temp = *this;

			l_temp.clamp();

			l_result += static_cast<bcUINT32>(255 * l_temp.z);
			l_result += (static_cast<bcUINT32>(255 * l_temp.y) <<8);
			l_result += (static_cast<bcUINT32>(255 * l_temp.x) <<16);
			l_result += (static_cast<bcUINT32>(255 * l_temp.w) <<24);

			return l_result;
		}

		template<typename T>
		bcUINT32 bc_vector4<T>::to_rgba() const noexcept
		{
			bcUINT32 l_result = 0;
			bc_vector4<T> l_temp = *this;

			l_temp.clamp();

			l_result += static_cast<bcUINT32>(255 * l_temp.w);
			l_result += (static_cast<bcUINT32>(255 * l_temp.z) <<8);
			l_result += (static_cast<bcUINT32>(255 * l_temp.y) <<16);
			l_result += (static_cast<bcUINT32>(255 * l_temp.x) <<24);

			return l_result;
		}

		template<typename T>
		void bc_vector4<T>::from_argb(bcUINT32 color) noexcept
		{
			x = static_cast<float>((color & 0x00ff0000)>> 16) / 255.0f; // red channel
			y = static_cast<float>((color & 0x0000ff00)>> 8) / 255.0f; // green channel
			z = static_cast<float>((color & 0x000000ff)) / 255.0f; // blue channel
			w = static_cast<float>((color & 0xff000000)>> 24) / 255.0f; // alpha channel
		}

		template<typename T>
		bc_vector3<T> bc_vector4<T>::xyz() const noexcept
		{
			return bc_vector3f(x, y, z);
		}

		template<typename T>
		bc_vector2<T> bc_vector4<T>::xy() const noexcept
		{
			return bc_vector2f(x, y);
		}
	}
}