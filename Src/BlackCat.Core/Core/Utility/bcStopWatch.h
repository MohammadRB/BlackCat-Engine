// [11/10/2020 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include <numeric>

namespace black_cat
{
	namespace core
	{
		class bc_stop_watch
		{
		public:
			bc_stop_watch() noexcept;

			bc_stop_watch(const bc_stop_watch&) = delete;

			bc_stop_watch(bc_stop_watch&&) = delete;

			~bc_stop_watch() = default;

			bc_stop_watch& operator=(const bc_stop_watch&) = delete;
			
			bc_stop_watch& operator=(bc_stop_watch&&) = delete;

			void start() noexcept;

			void stop() noexcept;

			void restart() noexcept;

			core_platform::bc_clock::small_delta_time total_elapsed() const noexcept;
			
			core_platform::bc_clock::small_delta_time average_total_elapsed() const noexcept;

		private:
			static const bcUINT32 s_num_samples = 64;
			core_platform::bc_clock::small_delta_time m_elapsed_millisecond_samples[s_num_samples];
			bcUINT32 m_current_sample;
			
			core_platform::bc_clock::big_clock m_clock_per_milliseconds;
			core_platform::bc_clock::big_clock m_start_clock;
			core_platform::bc_clock::small_delta_time m_elapsed_milliseconds;
		};

		inline bc_stop_watch::bc_stop_watch() noexcept
			: m_elapsed_millisecond_samples{0},
			m_current_sample(0),
			m_clock_per_milliseconds(core_platform::bc_clock::query_clock_per_millisecond()),
			m_start_clock(0),
			m_elapsed_milliseconds(0)
		{
			std::memset(&m_elapsed_millisecond_samples, 0, sizeof(core_platform::bc_clock::small_delta_time) * s_num_samples);
		}

		inline void bc_stop_watch::start() noexcept
		{
			m_start_clock = core_platform::bc_clock::query_elapsed_clocks();
		}

		inline void bc_stop_watch::stop() noexcept
		{
			bcAssert(m_start_clock != 0);
			
			const auto l_current_clock = core_platform::bc_clock::query_elapsed_clocks();
			const core_platform::bc_clock::big_delta_time l_elapsed_clocks = l_current_clock - m_start_clock;
			
			m_elapsed_milliseconds += l_elapsed_clocks / m_clock_per_milliseconds;
		}

		inline void bc_stop_watch::restart() noexcept
		{
			m_elapsed_millisecond_samples[m_current_sample] = m_elapsed_milliseconds;
			m_current_sample = (m_current_sample + 1) % s_num_samples;
			
			m_start_clock = 0;
			m_elapsed_milliseconds = 0;
		}

		inline core_platform::bc_clock::small_delta_time bc_stop_watch::total_elapsed() const noexcept
		{
			return m_elapsed_milliseconds;
		}

		inline core_platform::bc_clock::small_delta_time bc_stop_watch::average_total_elapsed() const noexcept
		{
			const auto l_sum = std::accumulate(std::begin(m_elapsed_millisecond_samples), std::end(m_elapsed_millisecond_samples), 0);
			const auto l_average = l_sum * 1. / s_num_samples;

			return l_average;
		}
	}
}