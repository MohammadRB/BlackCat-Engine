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
		class bc_clock_provider : private bc_no_copy_move
		{
		public:
			
		protected:

		private:
		};	

		template< class TProvider >
		class bc_clock_proxy : private bc_no_copy_move
		{
		private:
			using provider_type = TProvider;

		public:
			using large_clock_type = typename provider_type::large_clock_type;
			using little_clock_type = typename provider_type::little_clock_type;
			using large_time_delta_type = typename provider_type::large_time_delta_type;
			using little_time_delta_type = typename provider_type::little_time_delta_type;

		public:
			bc_clock_proxy() noexcept(true)
			{
			}

			~bc_clock_proxy() noexcept(true)
			{
			}

			// Number of cpu cycles that has been elapsed since clock startup
			large_clock_type clocks() const noexcept(true)
			{
				return m_provider.clocks();
			}
						
			// Total elapsed time by millisecond since clock startup
			large_time_delta_type total_elapsed() const noexcept(true)
			{
				return m_provider.total_elapsed();
			}

			// Elapsed time by millisecond from last update
			little_time_delta_type elapsed() const noexcept(true)
			{
				return m_provider.elapsed();
			}

			// Scale up/down clock progress
			void scale(little_time_delta_type p_scale) noexcept(true)
			{
				m_provider.scale(p_scale);
			}

			little_time_delta_type scale() const noexcept(true)
			{
				return m_provider.scale();
			}

			void pause() noexcept(true)
			{
				m_provider.pause();
			}

			bool paused() const noexcept(true)
			{
				return m_provider.paused();
			}

			// Specify a fixed step time by millisecond
			void fixed_step(little_time_delta_type p_delta) noexcept(true)
			{
				m_provider.fixed_step(p_delta);
			}

			// Fixed step time by millisecond if clock is in fixed step mode otherwise -1
			little_time_delta_type fixed_step() const noexcept(true)
			{
				return m_provider.fixed_step();
			}

			// Time difference between two clock by millisecond
			little_time_delta_type operator -(const bc_clock_proxy& p_other) noexcept(true)
			{
				return m_provider.get_delta_time(p_other.m_provider);
			}

			void update() noexcept(true)
			{
				m_provider.update();
			}

		protected:

		private:
			provider_type m_provider;
		};

		using bc_clock = bc_clock_proxy< bc_clock_provider< g_current_platform > >;
	}
}
