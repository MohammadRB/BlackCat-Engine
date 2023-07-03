// [13/03/2021 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"

namespace black_cat
{
	namespace core
	{
		template<typename T>
		class bc_velocity
		{
		public:
			constexpr bc_velocity(T p_min, T p_max, bcFLOAT p_drag_time) noexcept;

			bc_velocity(const bc_velocity&) noexcept;

			~bc_velocity();

			bc_velocity& operator=(const bc_velocity&) noexcept;

			bcFLOAT get_drag_time() const noexcept;
			
			void push(platform::bc_clock::small_time p_elapsed_seconds) noexcept;
			
			void release(platform::bc_clock::small_time p_elapsed_seconds) noexcept;

			T get_value() const noexcept;

			void set_value(T p_value) noexcept;
		private:
			T m_min;
			T m_max;
			bcFLOAT m_drag_time;
			bcFLOAT m_current_drag;
		};

		template<typename T>
		constexpr bc_velocity<T>::bc_velocity(T p_min, T p_max, bcFLOAT p_drag_time) noexcept
			: m_min(p_min),
			m_max(p_max),
			m_drag_time(p_drag_time),
			m_current_drag(0)
		{
		}

		template<typename T>
		bc_velocity<T>::bc_velocity(const bc_velocity&) noexcept = default;

		template<typename T>
		bc_velocity<T>::~bc_velocity() = default;

		template<typename T>
		bc_velocity<T>& bc_velocity<T>::operator=(const bc_velocity&) noexcept = default;

		template<typename T>
		bcFLOAT bc_velocity<T>::get_drag_time() const noexcept
		{
			return m_drag_time;
		}

		template<typename T>
		void bc_velocity<T>::push(platform::bc_clock::small_time p_elapsed_seconds) noexcept
		{
			m_current_drag = std::min(m_drag_time, m_current_drag + p_elapsed_seconds);
		}

		template<typename T>
		void bc_velocity<T>::release(platform::bc_clock::small_time p_elapsed_seconds) noexcept
		{
			m_current_drag = std::max(0.f, m_current_drag - p_elapsed_seconds);
		}

		template<typename T>
		T bc_velocity<T>::get_value() const noexcept
		{
			return m_min + (m_max - m_min) * (m_current_drag / m_drag_time);
		}

		template<typename T>
		void bc_velocity<T>::set_value(T p_value) noexcept
		{
			p_value = std::min(std::max(p_value, m_min), m_max);
			m_current_drag = ((p_value - m_min) / (m_max - m_min)) * m_drag_time;
		}
	}
}