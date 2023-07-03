// [11/01/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<typename TState>
		class bc_pipeline_state_variable
		{
		public:
			explicit bc_pipeline_state_variable(TState p_initial_state) noexcept;

			bc_pipeline_state_variable(const bc_pipeline_state_variable&) noexcept;

			~bc_pipeline_state_variable();

			bc_pipeline_state_variable& operator=(const bc_pipeline_state_variable&) noexcept;

			void set(TState p_state) noexcept;

			TState get() const noexcept;

			void set_to_initial_state() noexcept;

			bool update_needed() const noexcept;

			void reset_tracking() noexcept;

		private:
			const TState m_initial_state;
			TState m_state;

			bool m_update_needed;
		};

		template<typename TState>
		bc_pipeline_state_variable<TState>::bc_pipeline_state_variable(TState p_initial_state) noexcept
			: m_initial_state(p_initial_state),
			m_state(p_initial_state),
			m_update_needed(false)
		{
		}

		template<typename TState>
		bc_pipeline_state_variable<TState>::bc_pipeline_state_variable(const bc_pipeline_state_variable& p_other) noexcept
			: m_initial_state(p_other.m_initial_state),
			m_state(p_other.m_state),
			m_update_needed(p_other.m_update_needed)
		{
		}

		template<typename TState>
		bc_pipeline_state_variable<TState>::~bc_pipeline_state_variable()
		{
		}

		template<typename TState>
		bc_pipeline_state_variable<TState>& bc_pipeline_state_variable<TState>::operator=(const bc_pipeline_state_variable& p_other) noexcept
		{
			m_state = p_other.m_state;
			m_update_needed = p_other.m_update_needed;

			return *this;
		}

		template<typename TState>
		void bc_pipeline_state_variable<TState>::set(TState p_state) noexcept
		{
			if (p_state != m_state)
			{
				m_update_needed = true;
			}
			else
			{
				m_update_needed = false;
			}

			m_state = p_state;
		}

		template<typename TState>
		TState bc_pipeline_state_variable<TState>::get() const noexcept
		{
			return m_state;
		}

		template<typename TState>
		void bc_pipeline_state_variable<TState>::set_to_initial_state() noexcept
		{
			set(m_initial_state);
		}

		template<typename TState>
		bool bc_pipeline_state_variable<TState>::update_needed() const noexcept
		{
			return m_update_needed;
		}

		template<typename TState>
		void bc_pipeline_state_variable<TState>::reset_tracking() noexcept
		{
			m_update_needed = false;
		}
	}
}