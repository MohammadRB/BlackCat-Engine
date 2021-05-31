// [11/12/2020 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include <cstring>

namespace black_cat
{
	namespace core
	{
		template<typename TValue, bcUINT32 TSampleCount>
		class bc_value_sampler
		{
		public:
			bc_value_sampler(TValue p_default_value);

			bc_value_sampler(const bc_value_sampler&) noexcept;

			~bc_value_sampler();

			bc_value_sampler& operator=(const bc_value_sampler&) noexcept;

			void add_sample(TValue p_sample) noexcept;

			TValue average_value() const noexcept;

		private:
			TValue m_samples[TSampleCount];
			bcUINT32 m_current_sample;
		};

		template<typename TValue, bcUINT32 TSampleCount>
		bc_value_sampler<TValue, TSampleCount>::bc_value_sampler(TValue p_default_value)
			: m_current_sample(0)
		{
			std::memset(&m_samples, p_default_value, sizeof(TValue) * TSampleCount);
		}

		template<typename TValue, bcUINT32 TSampleCount>
		bc_value_sampler<TValue, TSampleCount>::bc_value_sampler(const bc_value_sampler& p_other) noexcept
		{
			operator=(p_other);
		}

		template<typename TValue, bcUINT32 TSampleCount>
		bc_value_sampler<TValue, TSampleCount>::~bc_value_sampler() = default;

		template<typename TValue, bcUINT32 TSampleCount>
		bc_value_sampler<TValue, TSampleCount>& bc_value_sampler<TValue, TSampleCount>::operator=(const bc_value_sampler& p_other) noexcept
		{
			std::memcpy(&m_samples, &p_other.m_samples, sizeof(TValue) * TSampleCount);
			m_current_sample = p_other.m_current_sample;
			return *this;
		}

		template<typename TValue, bcUINT32 TSampleCount>
		void bc_value_sampler<TValue, TSampleCount>::add_sample(TValue p_sample) noexcept
		{
			m_samples[m_current_sample] = p_sample;
			m_current_sample = (m_current_sample + 1) % TSampleCount;
		}

		template<typename TValue, bcUINT32 TSampleCount>
		TValue bc_value_sampler<TValue, TSampleCount>::average_value() const noexcept
		{
			const auto l_sum = std::accumulate(std::begin(m_samples), std::end(m_samples), static_cast<TValue>(0.));
			const auto l_average = l_sum / TSampleCount;

			return l_average;
		}
	}
}