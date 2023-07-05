// [11/01/2016 MRB]

#pragma once

#include "Core/Container/bcArray.h"
#include "Graphic/GraphicPCH.h"

namespace black_cat::graphic
{
	template<typename TState, bcUINT32 TNum>
	class bc_pipeline_state_array_variable
	{
	public:
		constexpr static bcUINT32 num_slots = TNum;
			
	public:
		explicit bc_pipeline_state_array_variable(TState p_initial_state) noexcept;

		bc_pipeline_state_array_variable(const bc_pipeline_state_array_variable&) noexcept;

		~bc_pipeline_state_array_variable();

		bc_pipeline_state_array_variable& operator=(const bc_pipeline_state_array_variable&) noexcept;

		void set_to_initial_state() noexcept;

		void set(bcUINT32 p_slot, TState p_state) noexcept;

		TState get(bcUINT32 p_slot) const noexcept;

		TState* get_slot(bcUINT32 p_slot) noexcept;

		TState* get_first_slot() noexcept;

		bcUINT32 get_dirty_start() const noexcept;

		bcUINT32 get_dirty_end() const noexcept;

		bcUINT32 get_dirty_count() const noexcept;

		bcUINT32 variable_count() const noexcept;

		bool update_needed() const noexcept;

		void reset_tracking() noexcept;

	private:
		const TState m_initial_state;
		core::bc_array<TState, TNum> m_states;

		bcINT32 m_dirty_start;
		bcINT32 m_dirty_end;
		bool m_update_needed;
	};

	template<typename TState, bcUINT32 TNum>
	bc_pipeline_state_array_variable<TState, TNum>::bc_pipeline_state_array_variable(TState p_initial_state) noexcept
		: m_initial_state(p_initial_state),
		  m_states{ m_initial_state },
		  m_dirty_start(-1),
		  m_dirty_end(-1),
		  m_update_needed(false)
	{
		m_states.fill(p_initial_state);
	}

	template<typename TState, bcUINT32 TNum>
	bc_pipeline_state_array_variable<TState, TNum>::bc_pipeline_state_array_variable(const bc_pipeline_state_array_variable& p_other) noexcept
		: m_initial_state(p_other.m_initial_state),
		  m_states(p_other.m_states),
		  m_dirty_start(p_other.m_dirty_start),
		  m_dirty_end(p_other.m_dirty_end),
		  m_update_needed(p_other.m_update_needed)
	{
	}

	template<typename TState, bcUINT32 TNum>
	bc_pipeline_state_array_variable<TState, TNum>::~bc_pipeline_state_array_variable() = default;

	template<typename TState, bcUINT32 TNum>
	bc_pipeline_state_array_variable<TState, TNum>& bc_pipeline_state_array_variable<TState, TNum>::operator=(const bc_pipeline_state_array_variable& p_other) noexcept
	{
		m_states = p_other.m_states;
		m_dirty_start = p_other.m_dirty_start;
		m_dirty_end = p_other.m_dirty_end;
		m_update_needed = p_other.m_update_needed;

		return *this;
	}

	template<typename TState, bcUINT32 TNum>
	void bc_pipeline_state_array_variable<TState, TNum>::set_to_initial_state() noexcept
	{
		for (bcUINT32 i = 0; i <TNum; ++i)
		{
			set(i, m_initial_state);
		}
	}

	template<typename TState, bcUINT32 TNum>
	void bc_pipeline_state_array_variable<TState, TNum>::set(bcUINT32 p_slot, TState p_state) noexcept
	{
		BC_ASSERT(p_slot <m_states.size());

		const bool l_is_same = m_states[p_slot] == p_state;

		if (!l_is_same)
		{
			if (!m_update_needed)
			{
				m_dirty_start = m_dirty_end = p_slot;
			}
			else if (p_slot <m_dirty_start)
			{
				m_dirty_start = p_slot;
			}
			else if (p_slot> m_dirty_end)
			{
				m_dirty_end = p_slot;
			}

			m_states[p_slot] = p_state;
			m_update_needed = true;
		}
	}

	template<typename TState, bcUINT32 TNum>
	TState bc_pipeline_state_array_variable<TState, TNum>::get(bcUINT32 p_slot) const noexcept
	{
		BC_ASSERT(p_slot <m_states.size());

		return m_states[p_slot];
	}

	template<typename TState, bcUINT32 TNum>
	TState* bc_pipeline_state_array_variable<TState, TNum>::get_slot(bcUINT32 p_slot) noexcept
	{
		BC_ASSERT(p_slot <m_states.size());

		return &m_states[p_slot];
	}

	template<typename TState, bcUINT32 TNum>
	TState* bc_pipeline_state_array_variable<TState, TNum>::get_first_slot() noexcept
	{
		return get_slot(0);
	}

	template<typename TState, bcUINT32 TNum>
	bcUINT32 bc_pipeline_state_array_variable<TState, TNum>::get_dirty_start() const noexcept
	{
		if (!update_needed())
		{
			return 0;
		}
		return m_dirty_start;
	}

	template<typename TState, bcUINT32 TNum>
	bcUINT32 bc_pipeline_state_array_variable<TState, TNum>::get_dirty_end() const noexcept
	{
		if (!update_needed())
		{
			return 0;
		}
		return m_dirty_end;
	}

	template<typename TState, bcUINT32 TNum>
	bcUINT32 bc_pipeline_state_array_variable<TState, TNum>::get_dirty_count() const noexcept
	{
		if (!update_needed())
		{
			return 0;
		}
		return get_dirty_end() - get_dirty_start() + 1;
	}

	template<typename TState, bcUINT32 TNum>
	bcUINT32 bc_pipeline_state_array_variable<TState, TNum>::variable_count() const noexcept
	{
		return TNum;
	}

	template<typename TState, bcUINT32 TNum>
	bool bc_pipeline_state_array_variable<TState, TNum>::update_needed() const noexcept
	{
		return m_update_needed;
	}

	template<typename TState, bcUINT32 TNum>
	void bc_pipeline_state_array_variable<TState, TNum>::reset_tracking() noexcept
	{
		m_update_needed = false;
		m_dirty_start = -1;
		m_dirty_end = -1;
	}
}
