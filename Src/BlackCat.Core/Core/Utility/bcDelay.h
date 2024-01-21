// [14/01/2024 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include <chrono>

namespace black_cat::core
{
	class bc_delay
	{
	public:
		bc_delay(std::chrono::milliseconds p_delay, bool p_trigger_on_start);

		bc_delay(const bc_delay&) noexcept = default;

		~bc_delay() = default;

		bc_delay& operator=(const bc_delay&) noexcept = default;

		[[nodiscard]]
		bool exceeded_on_last_update() const noexcept;

		bool update(const platform::bc_clock::update_param& p_clock) noexcept;

	private:
		platform::bc_clock::small_time m_delay;
		platform::bc_clock::small_time m_elapsed;
		bool m_delay_exceeded;
	};

	inline bc_delay::bc_delay(std::chrono::milliseconds p_delay, bool p_trigger_on_start)
		: m_delay(static_cast<platform::bc_clock::small_time>(p_delay.count())),
		m_elapsed(p_trigger_on_start ? static_cast<platform::bc_clock::small_time>(p_delay.count()) : 0.f),
		m_delay_exceeded(false)
	{
	}

	inline bool bc_delay::exceeded_on_last_update() const noexcept
	{
		return m_delay_exceeded;
	}

	inline bool bc_delay::update(const platform::bc_clock::update_param& p_clock) noexcept
	{
		m_elapsed += p_clock.m_elapsed;

		if (m_elapsed >= m_delay)
		{
			m_delay_exceeded = true;
			m_elapsed = 0;
		}
		else
		{
			m_delay_exceeded = false;
		}

		return m_delay_exceeded;
	}
}
