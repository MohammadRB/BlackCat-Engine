#pragma once

#include "CorePlatform/bcType.h"
#include "Core/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_logger_output_stream;
		
		template<typename T>
		class bc_vector2
		{
		public:
			bc_vector2() noexcept = default;

			explicit bc_vector2(T p_number) noexcept;

			bc_vector2(T p_x, T p_y) noexcept;

			bc_vector2(const bc_vector2& p_other) noexcept = default;

			~bc_vector2() = default;

			bc_vector2& operator=(const bc_vector2& p_other) noexcept = default;

			void clamp() noexcept;

			void normalize() noexcept;

			bcFLOAT magnitude() const noexcept;

			void make_zero() noexcept;

			T operator[](int p_pos) const noexcept;

			T& operator[](int p_pos) noexcept;

			bool operator==(const bc_vector2& p_other) const noexcept;

			bool operator!=(const bc_vector2& p_other) const noexcept;

			bc_vector2 operator+(const bc_vector2& p_other) const noexcept;

			bc_vector2 operator-(const bc_vector2& p_other) const noexcept;

			bc_vector2 operator*(const bc_vector2& p_other) const noexcept;

			bc_vector2 operator*(T p_scalar) const noexcept;

			bc_vector2 operator/(T p_scalar) const noexcept;

			bc_vector2 operator-() const noexcept;

			bc_vector2& operator+=(const bc_vector2& p_other) noexcept;

			bc_vector2& operator-=(const bc_vector2& p_other) noexcept;

			bc_vector2& operator*=(T p_scalar) noexcept;

			bc_vector2& operator/=(T p_scalar) noexcept;

		public:
			T x;
			T y;
		};

		using bc_vector2f = bc_vector2<bcFLOAT>;
		using bc_vector2i = bc_vector2<bcINT32>;

		BC_CORE_DLL bc_logger_output_stream& operator<<(bc_logger_output_stream& p_stream, const bc_vector2f& p_vector);
		
		BC_CORE_DLL bc_logger_output_stream& operator<<(bc_logger_output_stream& p_stream, const bc_vector2i& p_vector);
		
		template<typename T>
		bc_vector2<T>::bc_vector2(T p_number) noexcept
			: x(p_number),
			y(p_number)
		{
		}

		template<typename T>
		bc_vector2<T>::bc_vector2(T p_x, T p_y) noexcept
		{
			x = p_x;
			y = p_y;
		}
		
		template<typename T>
		void bc_vector2<T>::clamp() noexcept
		{
			if (x> 1.0f) x = 1.0f;
			if (x <0.0f) x = 0.0f;

			if (y> 1.0f) y = 1.0f;
			if (y <0.0f) y = 0.0f;
		}

		template<typename T>
		void bc_vector2<T>::normalize() noexcept
		{
			T l_inv_mag = (1.0f / magnitude());

			x *= l_inv_mag;
			y *= l_inv_mag;
		}

		template<typename T>
		bcFLOAT bc_vector2<T>::magnitude() const noexcept
		{
			bcFLOAT l_length = 0.0f;

			l_length += x * x;
			l_length += y * y;

			return std::sqrtf(l_length);
		}

		template<typename T>
		void bc_vector2<T>::make_zero() noexcept
		{
			x = 0.0f;
			y = 0.0f;
		}

		template<typename T>
		T bc_vector2<T>::operator[](int p_pos) const noexcept
		{
			if (p_pos == 0)
			{
				return x;
			}

			return y;
		}

		template<typename T>
		T& bc_vector2<T>::operator[](int p_pos) noexcept
		{
			if (p_pos == 0)
			{
				return x;
			}

			return y;
		}

		template<typename T>
		bool bc_vector2<T>::operator==(const bc_vector2<T>& p_other) const noexcept
		{
			if ((x - p_other.x) * (x - p_other.x)> 0.01f)
			{
				return false;
			}
			if ((y - p_other.y) * (y - p_other.y)> 0.01f)
			{
				return false;
			}

			return true;
		}

		template<typename T>
		bool bc_vector2<T>::operator!=(const bc_vector2<T>& p_other) const noexcept
		{
			return !(*this == p_other);
		}

		template<typename T>
		bc_vector2<T> bc_vector2<T>::operator+(const bc_vector2<T>& p_other) const noexcept
		{
			bc_vector2<T> l_sum;

			l_sum.x = x + p_other.x;
			l_sum.y = y + p_other.y;

			return l_sum;
		}

		template<typename T>
		bc_vector2<T> bc_vector2<T>::operator-(const bc_vector2<T>& p_other) const noexcept
		{
			bc_vector2<T> l_diff;

			l_diff.x = x - p_other.x;
			l_diff.y = y - p_other.y;

			return l_diff;
		}

		template<typename T>
		bc_vector2<T> bc_vector2<T>::operator*(const bc_vector2<T>& p_other) const noexcept
		{
			bc_vector2<T> l_prod;

			l_prod.x = x * p_other.x;
			l_prod.y = y * p_other.y;

			return l_prod;
		}

		template<typename T>
		bc_vector2<T> bc_vector2<T>::operator*(T p_scalar) const noexcept
		{
			bc_vector2<T> l_prod;

			l_prod.x = x * p_scalar;
			l_prod.y = y * p_scalar;

			return l_prod;
		}

		template<typename T>
		bc_vector2<T> bc_vector2<T>::operator/(T p_scalar) const noexcept
		{
			bc_vector2<T> l_quot;
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

		template<typename T>
		bc_vector2<T> bc_vector2<T>::operator-() const noexcept
		{
			bc_vector2<T> l_neg;

			l_neg.x = -x;
			l_neg.y = -y;

			return l_neg;
		}

		template<typename T>
		bc_vector2<T>& bc_vector2<T>::operator+=(const bc_vector2& p_other) noexcept
		{
			x += p_other.x;
			y += p_other.y;

			return *this;
		}

		template<typename T>
		bc_vector2<T>& bc_vector2<T>::operator-=(const bc_vector2& p_other) noexcept
		{
			x -= p_other.x;
			y -= p_other.y;

			return *this;
		}

		template<typename T>
		bc_vector2<T>& bc_vector2<T>::operator*=(T p_scalar) noexcept
		{
			x *= p_scalar;
			y *= p_scalar;

			return *this;
		}

		template<typename T>
		bc_vector2<T>& bc_vector2<T>::operator/=(T p_scalar) noexcept
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