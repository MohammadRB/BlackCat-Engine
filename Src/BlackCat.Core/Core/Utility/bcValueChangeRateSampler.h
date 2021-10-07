// [09/27/2021 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Container/bcArray.h"

namespace black_cat
{
	namespace core
	{
		template<bcUINT32 TValue>
		struct _bc_value_is_between_0_100
		{
			static constexpr bool value = TValue >= 0 && TValue <= 100;
		};
		
		/**
		 * \brief Calculate the rate of change of input samples based on different weights for each sample.
		 * \tparam T 
		 * \tparam TRatios Ratios must be between 0 - 100
		 */
		template<typename T, bcUINT32 ...TRatios>
		class bc_value_change_rate_sampler
		{
		public:
			explicit constexpr bc_value_change_rate_sampler(const T& p_default = {0});

			bc_value_change_rate_sampler(const bc_value_change_rate_sampler&) noexcept;

			~bc_value_change_rate_sampler();

			bc_value_change_rate_sampler& operator=(const bc_value_change_rate_sampler&) noexcept;

			void add_sample() noexcept;
			
			void add_sample(const T& p_sample) noexcept;

			T change_rate() const noexcept;
		
		private:
			bc_array<bcFLOAT, sizeof...(TRatios)> m_ratios;
			bc_array<T, sizeof...(TRatios) + 1> m_samples;
		};

		template<typename T, bcUINT32... TRatios>
		constexpr bc_value_change_rate_sampler<T, TRatios...>::bc_value_change_rate_sampler(const T& p_default)
			: m_ratios{ TRatios... }
		{
			static_assert(std::conjunction_v<_bc_value_is_between_0_100<TRatios>...>, "TRatios must be between 0 and 100");

			m_samples.fill(p_default);
			for(auto& l_ratio : m_ratios)
			{
				l_ratio /= 100;
			}
		}

		template<typename T, bcUINT32... TRatios>
		bc_value_change_rate_sampler<T, TRatios...>::bc_value_change_rate_sampler(const bc_value_change_rate_sampler&) noexcept = default;

		template<typename T, bcUINT32... TRatios>
		bc_value_change_rate_sampler<T, TRatios...>::~bc_value_change_rate_sampler() = default;

		template<typename T, bcUINT32... TRatios>
		bc_value_change_rate_sampler<T, TRatios...>& bc_value_change_rate_sampler<T, TRatios...>::operator=(const bc_value_change_rate_sampler&) noexcept = default;

		template<typename T, bcUINT32... TRatios>
		void bc_value_change_rate_sampler<T, TRatios...>::add_sample() noexcept
		{
			add_sample(static_cast<T>(m_samples[0]));
		}

		template<typename T, bcUINT32... TRatios>
		void bc_value_change_rate_sampler<T, TRatios...>::add_sample(const T& p_sample) noexcept
		{
			// rotate to the right
			std::rotate(std::rbegin(m_samples), std::rbegin(m_samples) + 1, std::rend(m_samples));
			m_samples.front() = p_sample;
		}

		template<typename T, bcUINT32... TRatios>
		T bc_value_change_rate_sampler<T, TRatios...>::change_rate() const noexcept
		{
			T l_change_ratio{ 0 };
			
			for(auto l_ite = 0U; l_ite < m_ratios.size(); ++l_ite)
			{
				auto l_ratio = m_ratios[l_ite];
				auto l_sample = m_samples[l_ite];
				auto l_prev_sample = m_samples[l_ite + 1];

				l_change_ratio += (l_sample - l_prev_sample) * l_ratio;
			}

			return l_change_ratio;
		}
	}	
}