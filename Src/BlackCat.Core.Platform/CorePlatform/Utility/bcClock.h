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
				update_param(big_delta_time p_total_elapsed, small_delta_time p_elapsed)
					: m_total_elapsed(p_total_elapsed),
					m_total_elapsed_second(p_total_elapsed / 1000),
					m_elapsed(p_elapsed),
					m_elapsed_second(p_elapsed / 1000)
				{
				}

				big_delta_time m_total_elapsed;
				big_delta_time m_total_elapsed_second;
				small_delta_time m_elapsed;
				small_delta_time m_elapsed_second;
			};

		public:
			bc_platform_clock() noexcept;

			~bc_platform_clock() noexcept;

			/**
			 * \brief Number of cpu cycles that has been elapsed since clock startup
			 * \return 
			 */
			big_clock get_clocks() const noexcept
			{
				return m_clocks;
			}

			/**
			 * \brief Total elapsed time by millisecond since clock startup(This is a real value)
			 * \return 
			 */
			big_delta_time get_total_elapsed() const noexcept
			{
				return _clocks_to_milliseconds(m_clocks - m_start_up_clocks);
			}

			/**
			 * \brief Elapsed time by millisecond from last update(This can be scaled or limited)
			 * \return 
			 */
			small_delta_time get_elapsed() const noexcept
			{
				return m_elapsed;
			}

			/**
			 * \brief Scale up/down clock progress
			 * \param p_scale 
			 */
			void set_scale(small_delta_time p_scale) noexcept
			{
				m_scale = p_scale;
			}

			small_delta_time get_scale() const noexcept
			{
				return m_scale;
			}

			bool get_pause() const noexcept
			{
				return m_paused;
			}

			void pause() noexcept
			{
				m_paused = true;
			}

			void resume()
			{
				m_clocks = _query_elapsed_clocks();
				m_paused = false;
			}

			/**
			 * \brief Specify a fixed step time by millisecond
			 * \param p_delta 
			 */
			void fixed_step(small_delta_time p_delta) noexcept
			{
				if (p_delta < 0)
				{
					p_delta = -1;
				}

				m_fixed_step = p_delta;
			}

			/**
			 * \brief Fixed step time by millisecond if clock is in fixed step mode otherwise -1
			 * \return 
			 */
			small_delta_time fixed_step() const noexcept
			{
				return m_fixed_step;
			}

			/**
			 * \brief Time difference between two clock by millisecond
			 * \param p_other 
			 * \return 
			 */
			small_delta_time get_delta_time(const bc_platform_clock& p_other)
			{
				return static_cast<small_delta_time>(_clocks_to_milliseconds(m_clocks - p_other.m_clocks));
			}

			small_delta_time operator -(const bc_platform_clock& p_other) noexcept
			{
				return get_delta_time(p_other);
			}

			void update() noexcept;

		protected:

		private:
			big_clock _query_clock_per_millisecond();

			big_clock _query_elapsed_clocks();

			inline big_clock _milliseconds_to_clocks(small_delta_time p_seconds) const
			{
				return static_cast< big_clock >(p_seconds * m_clock_per_millisecond);
			}

			inline big_delta_time _clocks_to_milliseconds(big_clock p_clocks) const
			{
				return static_cast<big_delta_time>(p_clocks / m_clock_per_millisecond);
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
		bc_platform_clock< TPlatform >::bc_platform_clock() noexcept
		{
			m_clock_per_millisecond = _query_clock_per_millisecond();
			m_clocks = _query_elapsed_clocks();
			m_start_up_clocks = m_clocks;
			m_fixed_step = -1;
			m_scale = 1;
			m_paused = false;
		}

		template< bc_platform TPlatform >
		bc_platform_clock< TPlatform >::~bc_platform_clock() noexcept
		{
		}

		template< bc_platform TPlatform >
		void bc_platform_clock< TPlatform >::update() noexcept
		{
			if (m_paused) 
			{
				return;
			}

			const bool l_is_fixed = m_fixed_step != -1;
			big_clock l_current_clock = _query_elapsed_clocks();

			if (!l_is_fixed)
			{
				m_elapsed = static_cast<small_delta_time>(_clocks_to_milliseconds((l_current_clock - m_clocks)) * m_scale);
			}
			else
			{
				m_elapsed = m_fixed_step * m_scale;
			}

			m_clocks = l_current_clock;
		}
	}
}