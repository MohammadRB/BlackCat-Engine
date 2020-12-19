// [11/13/2020 MRB]

#pragma once

#include "Core/Math/bcVector2f.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcVector4f.h"
#include "Core/Container/bcVector.h"
#include <initializer_list>
#include <utility>
#include <iterator>
#include <algorithm>

namespace black_cat
{
	namespace core
	{
		template< typename T >
		class bc_curve
		{
		private:
			using point_container = bc_vector_movable<std::pair<bcFLOAT, T>>;
			using point_value_t = typename point_container::value_type;
			
		public:
			bc_curve(const std::initializer_list<T>& p_initializer);

			template<typename TIte>
			bc_curve(TIte p_begin, TIte p_end);

			bc_curve(bc_curve&&) noexcept;

			~bc_curve();

			bc_curve& operator=(bc_curve&&) noexcept;

			T sample(bcFLOAT p_time) const noexcept;

			bc_vector_frame<T> sample_many(bcUINT32 p_sample_count) const noexcept;

		private:
			static T _quadratic_curve(const point_value_t& p_first, const point_value_t& p_second, const point_value_t& p_third, bcFLOAT p_time) noexcept;
			
			template<typename T1>
			static T1 _linear_curve(const T1& p_first, const T1& p_second, bcFLOAT p_time) noexcept;

			static point_value_t _linear_curve(const point_value_t& p_first, const point_value_t& p_second, bcFLOAT p_time) noexcept;

			static bcFLOAT _clamp_time(bcFLOAT p_t1, bcFLOAT p_t2, bcFLOAT p_time) noexcept;

			point_container m_points;
		};

		using bc_curve1f = bc_curve<bcFLOAT>;
		using bc_curve2f = bc_curve<bc_vector2f>;
		using bc_curve3f = bc_curve<bc_vector3f>;
		using bc_curve4f = bc_curve<bc_vector4f>;

		template< typename T >
		bc_curve<T>::bc_curve(const std::initializer_list<T>& p_initializer)
			: bc_curve(std::begin(p_initializer), std::end(p_initializer))
		{
		}

		template< typename T >
		template< typename TIte >
		bc_curve<T>::bc_curve(TIte p_begin, TIte p_end)
		{
			auto l_point_count = std::distance(p_begin, p_end);
			m_points.reserve(l_point_count);

			bcSIZE l_counter = 0;
			bcFLOAT l_counter_multiplier = 1.f / (l_point_count - 1);
			std::transform
			(
				p_begin,
				p_end,
				std::back_inserter(m_points),
				[&](const T& p_point)
				{
					return std::make_pair(l_counter++ * l_counter_multiplier, p_point);
				}
			);
			
			if(m_points.size() == 0)
			{
				m_points.push_back(std::make_pair(0.f, T(0)));
			}

			if(m_points.size() == 1)
			{
				m_points.push_back(m_points[0]);
			}
		}

		template< typename T >
		bc_curve<T>::bc_curve(bc_curve&&) noexcept = default;

		template< typename T >
		bc_curve<T>::~bc_curve() = default;

		template< typename T >
		bc_curve<T>& bc_curve<T>::operator=(bc_curve&&) noexcept = default;

		template< typename T >
		T bc_curve<T>::sample(bcFLOAT p_time) const noexcept
		{
			p_time = std::min(1.f, std::max(0.f, p_time));
			
			auto l_lower_bound = std::lower_bound
			(
				std::begin(m_points),
				std::end(m_points),
				std::make_pair(p_time, T(0)),
				[](const point_value_t& p_point1, const point_value_t& p_point2)
				{
					return p_point1.first < p_point2.first;
				}
			);

			bcAssert(l_lower_bound != std::end(m_points));

			auto l_preceding_ite = l_lower_bound != std::begin(m_points) ? l_lower_bound - 1 : l_lower_bound;
			auto l_proceeding_ite = l_preceding_ite + 1;
			auto l_normalized_time = _clamp_time(l_preceding_ite->first, l_proceeding_ite->first, p_time);

			if(l_preceding_ite == std::begin(m_points) && l_normalized_time <= 0.5f)
			{
				auto l_middle_point = _linear_curve(*l_preceding_ite, *l_proceeding_ite, .5f);
				l_normalized_time = _clamp_time(l_preceding_ite->first, l_middle_point.first, p_time);
				
				auto l_curve_point = _linear_curve(*l_preceding_ite, l_middle_point, l_normalized_time);
				return l_curve_point.second;
			}

			if(l_proceeding_ite + 1 == std::end(m_points) && l_normalized_time >= 0.5f)
			{
				auto l_middle_point = _linear_curve(*l_preceding_ite, *l_proceeding_ite, .5f);
				l_normalized_time = _clamp_time(l_middle_point.first, l_proceeding_ite->first, p_time);

				auto l_curve_point = _linear_curve(l_middle_point, *l_proceeding_ite, l_normalized_time);
				return l_curve_point.second;
			}
			
			point_value_t l_first_point;
			point_value_t l_second_point;
			point_value_t l_third_point;
			
			if(l_normalized_time <= 0.5f)
			{
				l_first_point = _linear_curve(*(l_preceding_ite - 1), *l_preceding_ite, .5f);
				l_second_point = *l_preceding_ite;
				l_third_point = _linear_curve(*l_preceding_ite, *l_proceeding_ite, .5f);
			}
			else
			{
				l_first_point = _linear_curve(*l_preceding_ite, *l_proceeding_ite, .5f);
				l_second_point = *l_proceeding_ite;
				l_third_point = _linear_curve(*l_proceeding_ite, *(l_proceeding_ite + 1), .5f);
			}

			auto l_curve_point = _quadratic_curve(l_first_point, l_second_point, l_third_point, p_time);
			return l_curve_point;
		}

		template< typename T >
		bc_vector_frame<T> bc_curve<T>::sample_many(bcUINT32 p_sample_count) const noexcept
		{
			bc_vector_frame<T> l_result;
			l_result.reserve(p_sample_count);

			l_result.push_back(sample(0));
			for (bcSIZE l_i = 1; l_i < p_sample_count - 1; ++l_i)
			{
				l_result.push_back(sample(static_cast<bcFLOAT>(l_i) / (static_cast<bcFLOAT>(p_sample_count) - 1)));
			}
			l_result.push_back(sample(1));

			return l_result;
		}

		template< typename T >
		T bc_curve<T>::_quadratic_curve(const point_value_t& p_first, const point_value_t& p_second, const point_value_t& p_third, bcFLOAT p_time) noexcept
		{
			auto l_time = _clamp_time(p_first.first, p_third.first, p_time);
			auto l_mid_point1 = _linear_curve(p_first.second, p_second.second, l_time);
			auto l_mid_point2 = _linear_curve(p_second.second, p_third.second, l_time);
			auto l_curve_point = _linear_curve(l_mid_point1, l_mid_point2, l_time);
			return l_curve_point;
		}

		template< typename T >
		template< typename T1 >
		T1 bc_curve<T>::_linear_curve(const T1& p_first, const T1& p_second, bcFLOAT p_time) noexcept
		{
			bcAssert(p_time >= 0 && p_time <= 1);
			
			return p_first * (1.f - p_time) + p_second * p_time;
		}

		template< typename T >
		typename bc_curve<T>::point_value_t bc_curve<T>::_linear_curve(const point_value_t& p_first, const point_value_t& p_second, bcFLOAT p_time) noexcept
		{
			auto l_time = _linear_curve<bcFLOAT>(p_first.first, p_second.first, p_time);
			auto l_value = _linear_curve<T>(p_first.second, p_second.second, p_time);

			return std::make_pair(l_time, l_value);
		}

		template< typename T >
		bcFLOAT bc_curve< T >::_clamp_time(bcFLOAT p_t1, bcFLOAT p_t2, bcFLOAT p_time) noexcept
		{
			bcAssert(p_time >= p_t1 && p_time <= p_t2);

			return (p_time - p_t1) * (1.f / (p_t2 - p_t1)); // clamp time to [0,1] range
		}
	}
}
