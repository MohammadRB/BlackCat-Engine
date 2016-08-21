// [2/13/2015 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/Utility/bcNoCopy.h"

namespace black_cat
{
	namespace core_platform
	{
		template< bc_platform TPlatform >
		struct bc_platform_clock_pack
		{
		};

		template< bc_platform TPlatform >
		class bc_platform_clock : private bc_no_copy_move
		{
		public:
			using platform_pack = bc_platform_clock_pack< TPlatform >;
			using big_clock = typename platform_pack::big_clock;
			using small_clock = typename platform_pack::small_clock;
			using big_delta_time = typename platform_pack::big_delta_time;
			using small_delta_time = typename platform_pack::small_delta_time;

			struct update_param
			{
				update_param(big_clock p_total_elapsed, small_delta_time p_elapsed)
					: m_total_elapsed(p_total_elapsed),
					m_elapsed(p_elapsed)
				{
				}

				big_clock m_total_elapsed;
				small_delta_time m_elapsed;
			};

		public:
			bc_platform_clock() noexcept(true);

			~bc_platform_clock() noexcept(true);

			// Number of cpu cycles that has been elapsed since clock startup
			big_clock get_clocks() const noexcept(true)
			{
				return m_clocks;
			}

			// Total elapsed time by millisecond since clock startup(This is a real value)
			big_delta_time get_total_elapsed() const noexcept(true)
			{
				return _clocks_to_milliseconds(m_clocks - m_start_up_clocks);
			}

			// Elapsed time by millisecond from last update(This can be scaled or limited)
			small_delta_time get_elapsed() const noexcept(true)
			{
				return m_elapsed;
			}

			// Scale up/down clock progress
			void set_scale(small_delta_time p_scale) noexcept(true)
			{
				m_scale = p_scale;
			}

			small_delta_time get_scale() const noexcept(true)
			{
				return m_scale;
			}

			bool get_pause() const noexcept(true)
			{
				return m_paused;
			}

			void pause() noexcept(true)
			{
				m_paused = true;
			}

			void resume()
			{
				m_clocks = _query_elapsed_clocks();
				m_paused = false;
			}

			// Specify a fixed step time by millisecond
			void fixed_step(small_delta_time p_delta) noexcept(true)
			{
				if (p_delta < 0)
					p_delta = -1;

				m_fixed_step = p_delta;
			}

			// Fixed step time by millisecond if clock is in fixed step mode otherwise -1
			small_delta_time fixed_step() const noexcept(true)
			{
				return m_fixed_step;
			}

			// Time difference between two clock by millisecond
			small_delta_time get_delta_time(const bc_platform_clock& p_other)
			{
				return _clocks_to_milliseconds(m_clocks - p_other.m_clocks);
			}

			small_delta_time operator -(const bc_platform_clock& p_other) noexcept(true)
			{
				return get_delta_time(p_other);
			}

			void update() noexcept(true);

		protected:

		private:
			big_clock _query_clock_per_millisecond();

			big_clock _query_elapsed_clocks();

			inline big_clock _milliseconds_to_clocks(small_delta_time p_seconds) const
			{
				return static_cast< big_clock >(p_seconds * m_clock_per_millisecond);
			}

			// Use for short period of time
			inline small_delta_time _clocks_to_milliseconds(big_clock p_clocks) const
			{
				return static_cast< small_delta_time >(p_clocks) / m_clock_per_millisecond;
			}

			big_clock m_clock_per_millisecond;
			big_clock m_start_up_clocks;
			big_clock m_clocks;
			small_delta_time m_fixed_step;
			small_delta_time m_scale;
			small_delta_time m_elapsed;
			bool m_paused;
		};

		using bc_clock = bc_platform_clock< g_current_platform >;

		template< bc_platform TPlatform >
		bc_platform_clock< TPlatform >::bc_platform_clock() noexcept(true)
		{
			m_clock_per_millisecond = _query_clock_per_millisecond();
			m_clocks = _query_elapsed_clocks();
			m_start_up_clocks = m_clocks;
			m_fixed_step = -1;
			m_scale = 1;
			m_paused = false;
		}

		template< bc_platform TPlatform >
		bc_platform_clock< TPlatform >::~bc_platform_clock() noexcept(true)
		{
		}

		template< bc_platform TPlatform >
		void bc_platform_clock< TPlatform >::update() noexcept(true)
		{
			if (m_paused) return;

			bool l_is_fixed = m_fixed_step != -1;
			big_clock l_current_clock;

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
	}
}
