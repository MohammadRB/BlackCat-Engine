// [2/13/2015 MRB]

#pragma once

#include "CorePlatformImp/CoreWin32PCH.h"
#include "CorePlatform/Utility/bcClock.h"

namespace black_cat
{
	namespace core_platform
	{
		template< >
		class bc_clock_provider< bc_platform::win32 > : private bc_clock_provider< bc_platform::unknown >
		{
		public:
			using large_clock_type = bcUINT64;
			using little_clock_type = bcUINT32;
			using large_time_delta_type = bcDOUBLE64;
			using little_time_delta_type = bcFLOAT32;

		public:
			bc_clock_provider()
			{
				QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_clock_per_millisecond));
				m_clock_per_millisecond /= 1000;
				QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_clocks));
				m_fixed_step = -1;
				m_scale = 1;
			}

			~bc_clock_provider()
			{
			}

			large_clock_type clocks() const
			{
				return m_clocks;
			}

			large_time_delta_type total_elapsed() const
			{
				return _clocks_to_milliseconds(m_clocks);
			}

			little_time_delta_type elapsed() const
			{
				return m_elapsed;
			}

			void scale(little_time_delta_type p_scale)
			{
				m_scale = p_scale;
			}

			little_time_delta_type scale() const
			{
				return m_scale;
			}

			void pause()
			{
				m_fixed_step = 0;
			}

			bool paused() const
			{
				return m_fixed_step != 0;
			}

			void fixed_step(little_time_delta_type p_delta)
			{
				m_fixed_step = p_delta;
			}

			little_time_delta_type fixed_step() const
			{
				return m_fixed_step;
			}

			little_time_delta_type get_delta_time(const bc_clock_provider& p_other)
			{
				return _clocks_to_milliseconds(m_clocks - p_other.m_clocks);
			}

			void update()
			{
				bool l_is_fixed = m_fixed_step != -1;
				large_clock_type l_current_clock;

				QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&l_current_clock));

				if (!l_is_fixed)
				{
					m_elapsed = _clocks_to_milliseconds((m_clocks - l_current_clock)) * m_scale;
				}
				else
				{
					m_elapsed = m_fixed_step * m_scale;
				}

				m_clocks = l_current_clock;
			}

		protected:

		private:
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
		};
	}
}