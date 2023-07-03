// [10/11/2020 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Utility/bcValueSampler.h"

namespace black_cat
{
	namespace core
	{
		class bc_stop_watch
		{
		public:
			bc_stop_watch() noexcept;

			bc_stop_watch(const bc_stop_watch&) = default;

			~bc_stop_watch() = default;

			bc_stop_watch& operator=(const bc_stop_watch&) = default;

			void start() noexcept;

			void stop() noexcept;

			platform::bc_clock::small_time restart() noexcept;

			platform::bc_clock::small_time total_elapsed() const noexcept;
			
			platform::bc_clock::small_time average_total_elapsed() const noexcept;

		private:
			bc_value_sampler<platform::bc_clock::small_time> m_sampler;
			
			platform::bc_clock::big_clock m_clock_per_milliseconds;
			platform::bc_clock::big_clock m_start_clock;
			platform::bc_clock::small_time m_elapsed_milliseconds;
		};

		inline bc_stop_watch::bc_stop_watch() noexcept
			: m_sampler(0),
			m_clock_per_milliseconds(platform::bc_clock::query_clock_per_millisecond()),
			m_start_clock(0),
			m_elapsed_milliseconds(0)
		{
		}

		inline void bc_stop_watch::start() noexcept
		{
			m_start_clock = platform::bc_clock::query_elapsed_clocks();
		}

		inline void bc_stop_watch::stop() noexcept
		{
			BC_ASSERT(m_start_clock != 0);
			
			const auto l_current_clock = platform::bc_clock::query_elapsed_clocks();
			const platform::bc_clock::big_time l_elapsed_clocks = l_current_clock - m_start_clock;
			
			m_elapsed_milliseconds += l_elapsed_clocks / m_clock_per_milliseconds;
		}

		inline platform::bc_clock::small_time bc_stop_watch::restart() noexcept
		{
			const auto l_elapsed_milliseconds = m_elapsed_milliseconds;
			
			m_sampler.add_sample(m_elapsed_milliseconds);
			m_start_clock = 0;
			m_elapsed_milliseconds = 0;

			return l_elapsed_milliseconds;
		}

		inline platform::bc_clock::small_time bc_stop_watch::total_elapsed() const noexcept
		{
			return m_elapsed_milliseconds;
		}

		inline platform::bc_clock::small_time bc_stop_watch::average_total_elapsed() const noexcept
		{
			return m_sampler.average_value();
		}
	}
}