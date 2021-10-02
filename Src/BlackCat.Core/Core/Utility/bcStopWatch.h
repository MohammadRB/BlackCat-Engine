// [11/10/2020 MRB]

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
			bc_value_sampler<core_platform::bc_clock::small_delta_time, 64> m_sampler;
			
			core_platform::bc_clock::big_clock m_clock_per_milliseconds;
			core_platform::bc_clock::big_clock m_start_clock;
			core_platform::bc_clock::small_delta_time m_elapsed_milliseconds;
		};

		inline bc_stop_watch::bc_stop_watch() noexcept
			: m_sampler(0),
			m_clock_per_milliseconds(core_platform::bc_clock::query_clock_per_millisecond()),
			m_start_clock(0),
			m_elapsed_milliseconds(0)
		{
		}

		inline void bc_stop_watch::start() noexcept
		{
			m_start_clock = core_platform::bc_clock::query_elapsed_clocks();
		}

		inline void bc_stop_watch::stop() noexcept
		{
			BC_ASSERT(m_start_clock != 0);
			
			const auto l_current_clock = core_platform::bc_clock::query_elapsed_clocks();
			const core_platform::bc_clock::big_delta_time l_elapsed_clocks = l_current_clock - m_start_clock;
			
			m_elapsed_milliseconds += l_elapsed_clocks / m_clock_per_milliseconds;
		}

		inline void bc_stop_watch::restart() noexcept
		{
			m_sampler.add_sample(m_elapsed_milliseconds);
			
			m_start_clock = 0;
			m_elapsed_milliseconds = 0;
		}

		inline core_platform::bc_clock::small_delta_time bc_stop_watch::total_elapsed() const noexcept
		{
			return m_elapsed_milliseconds;
		}

		inline core_platform::bc_clock::small_delta_time bc_stop_watch::average_total_elapsed() const noexcept
		{
			return m_sampler.average_value();
		}
	}
}