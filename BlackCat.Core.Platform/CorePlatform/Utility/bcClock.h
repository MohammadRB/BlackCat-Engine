// [2/13/2015 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/bcCorePlatformUtility.h"

namespace black_cat
{
	namespace core_platform
	{
		template< bc_platform TPlatform >
		struct bc_clock_pack
		{
		};

		template< bc_platform TPlatform >
		class bc_clock_proxy : private bc_no_copy_move
		{
		public:
			using pack = bc_clock_pack<TPlatform>;
			using large_clock_type = typename pack::large_clock_type;
			using little_clock_type = typename pack::little_clock_type;
			using large_time_delta_type = typename pack::large_time_delta_type;
			using little_time_delta_type = typename pack::little_time_delta_type;

		public:
			bc_clock_proxy() noexcept(true)
			{
				m_clock_per_millisecond = _query_clock_per_millisecond();
				m_clocks = _query_elapsed_clocks();
				m_fixed_step = -1;
				m_scale = 1;
				m_paused = false;
			}

			~bc_clock_proxy() noexcept(true)
			{
			}

			// Number of cpu cycles that has been elapsed since clock startup
			large_clock_type clocks() const noexcept(true)
			{
				return m_clocks;
			}

			// Total elapsed time by millisecond since clock startup
			large_time_delta_type total_elapsed() const noexcept(true)
			{
				return _clocks_to_milliseconds(m_clocks);
			}

			// Elapsed time by millisecond from last update
			little_time_delta_type elapsed() const noexcept(true)
			{
				return m_elapsed;
			}

			// Scale up/down clock progress
			void scale(little_time_delta_type p_scale) noexcept(true)
			{
				m_scale = p_scale;
			}

			little_time_delta_type scale() const noexcept(true)
			{
				return m_scale;
			}

			void pause() noexcept(true)
			{
				m_paused = true;
			}

			bool paused() const noexcept(true)
			{
				return m_paused;
			}

			void resume()
			{
				m_clocks = _query_elapsed_clocks();
				m_paused = false;
			}

			// Specify a fixed step time by millisecond
			void fixed_step(little_time_delta_type p_delta) noexcept(true)
			{
				if (p_delta < 0)
					p_delta = -1;

				m_fixed_step = p_delta;
			}

			// Fixed step time by millisecond if clock is in fixed step mode otherwise -1
			little_time_delta_type fixed_step() const noexcept(true)
			{
				return m_fixed_step;
			}

			// Time difference between two clock by millisecond
			little_time_delta_type get_delta_time(const bc_clock_proxy& p_other)
			{
				return _clocks_to_milliseconds(m_clocks - p_other.m_clocks);
			}

			little_time_delta_type operator -(const bc_clock_proxy& p_other) noexcept(true)
			{
				return get_delta_time(p_other);
			}

			void update() noexcept(true)
			{
				if (m_paused) return;

				bool l_is_fixed = m_fixed_step != -1;
				large_clock_type l_current_clock;

				l_current_clock = _query_elapsed_clocks();

				if (!l_is_fixed)
				{
					m_elapsed = _clocks_to_milliseconds((l_current_clock - m_clocks)) * m_scale;
				}
				else
				{
					m_elapsed = m_fixed_step * m_scale;
				}

				m_clocks = l_current_clock;
			}

		protected:

		private:
			large_clock_type _query_clock_per_millisecond();

			large_clock_type _query_elapsed_clocks();

			inline large_clock_type _milliseconds_to_clocks(little_time_delta_type p_seconds) const
			{
				return static_cast<large_clock_type>(p_seconds * m_clock_per_millisecond);
			}

			// Use for short period of time
			inline little_time_delta_type _clocks_to_milliseconds(large_clock_type p_clocks) const
			{
				return static_cast<little_time_delta_type>(p_clocks) / m_clock_per_millisecond;
			}

			large_clock_type m_clock_per_millisecond;
			large_clock_type m_clocks;
			little_time_delta_type m_fixed_step;
			little_time_delta_type m_scale;
			little_time_delta_type m_elapsed;
			bool m_paused;
		};

		using bc_clock = bc_clock_proxy< g_current_platform >;
	}
}
