// [11/12/2020 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Container/bcArray.h"

namespace black_cat
{
	namespace core
	{
		template<typename TValue, bcUINT32 TSampleCount = 64>
		class bc_value_sampler
		{
		public:
			explicit bc_value_sampler(TValue p_default_value);

			bc_value_sampler(const bc_value_sampler&) noexcept;

			~bc_value_sampler();

			bc_value_sampler& operator=(const bc_value_sampler&) noexcept;

			void add_sample(TValue p_sample) noexcept;

			TValue average_value() const noexcept;

		private:
			bc_array<TValue, TSampleCount> m_samples;
			bcUINT32 m_current_sample;
		};

		template<typename TValue, bcUINT32 TSampleCount>
		bc_value_sampler<TValue, TSampleCount>::bc_value_sampler(TValue p_default_value)
			: m_current_sample(0)
		{
			m_samples.fill(p_default_value);
		}

		template<typename TValue, bcUINT32 TSampleCount>
		bc_value_sampler<TValue, TSampleCount>::bc_value_sampler(const bc_value_sampler& p_other) noexcept
			: m_samples(p_other.m_samples),
			m_current_sample(p_other.m_current_sample)
		{
		}

		template<typename TValue, bcUINT32 TSampleCount>
		bc_value_sampler<TValue, TSampleCount>::~bc_value_sampler() = default;

		template<typename TValue, bcUINT32 TSampleCount>
		bc_value_sampler<TValue, TSampleCount>& bc_value_sampler<TValue, TSampleCount>::operator=(const bc_value_sampler& p_other) noexcept
		{
			m_samples = p_other.m_samples;
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