// [01/11/2016 MRB]

#pragma once

#include "Core/Container/bcArray.h"
#include "Graphic/GraphicPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template< typename TState, bcUINT TNum >
		class bc_pipeline_state_array_variable
		{
		public:
			explicit bc_pipeline_state_array_variable(TState p_initial_state);

			bc_pipeline_state_array_variable(const bc_pipeline_state_array_variable&);

			~bc_pipeline_state_array_variable();

			bc_pipeline_state_array_variable& operator=(const bc_pipeline_state_array_variable&);

			void set_to_initial_state();

			void set(bcUINT p_slot, TState p_state);

			TState get(bcUINT p_slot) const;

			// Get pointer to internal memory of object(all objects are in continues memory)
			TState* get_slot(bcUINT p_slot);

			TState* get_first_slot();

			bcUINT get_dirty_start() const;

			bcUINT get_dirty_end() const;

			bcUINT get_dirty_count() const;

			bcUINT variable_count() const;

			bool update_needed() const;

			void reset_tracking();

		protected:

		private:
			const TState m_initial_state;
			core::bc_array< TState, TNum > m_states;

			bcINT m_dirty_start;
			bcINT m_dirty_end;
			bool m_update_needed;
		};

		template< typename TState, bcUINT TNum >
		bc_pipeline_state_array_variable< TState, TNum >::bc_pipeline_state_array_variable(TState p_initial_state)
			: m_initial_state(p_initial_state),
			m_states(),
			m_dirty_start(-1),
			m_dirty_end(-1),
			m_update_needed(false)
		{
			m_states.assign(p_initial_state);
		}

		template< typename TState, bcUINT TNum >
		bc_pipeline_state_array_variable<TState, TNum>::bc_pipeline_state_array_variable(const bc_pipeline_state_array_variable& p_other)
			: m_initial_state(p_other.m_initial_state),
			m_states(p_other.m_states),
			m_dirty_start(p_other.m_dirty_start),
			m_dirty_end(p_other.m_dirty_end),
			m_update_needed(p_other.m_update_needed)
		{
		}

		template< typename TState, bcUINT TNum >
		bc_pipeline_state_array_variable< TState, TNum >::~bc_pipeline_state_array_variable()
		{
		}

		template< typename TState, bcUINT TNum >
		bc_pipeline_state_array_variable<TState, TNum>& bc_pipeline_state_array_variable<TState, TNum>::operator=(const bc_pipeline_state_array_variable& p_other)
		{
			m_states = p_other.m_states;
			m_dirty_start = p_other.m_dirty_start;
			m_dirty_end = p_other.m_dirty_end;
			m_update_needed = p_other.m_update_needed;

			return *this;
		}

		template< typename TState, bcUINT TNum >
		void bc_pipeline_state_array_variable< TState, TNum >::set_to_initial_state()
		{
			for (bcUINT i = 0; i < TNum; ++i)
			{
				set(i, m_initial_state);
			}
		}

		template< typename TState, bcUINT TNum >
		void bc_pipeline_state_array_variable< TState, TNum >::set(bcUINT p_slot, TState p_state)
		{
			bcAssert(p_slot < m_states.size());

			bool l_is_same = m_states[p_slot] == p_state;

			if (!l_is_same)
			{
				if (!m_update_needed)
					m_dirty_start = m_dirty_end = p_slot;
				else if (p_slot < m_dirty_start)
					m_dirty_start = p_slot;
				else if (p_slot > m_dirty_end)
					m_dirty_end = p_slot;

				m_states[p_slot] = p_state;
				m_update_needed = true;
			}
		}

		template< typename TState, bcUINT TNum >
		TState bc_pipeline_state_array_variable< TState, TNum >::get(bcUINT p_slot) const
		{
			bcAssert(p_slot < m_states.size());

			return m_states[p_slot];
		}

		template< typename TState, bcUINT TNum >
		TState* bc_pipeline_state_array_variable< TState, TNum >::get_slot(bcUINT p_slot)
		{
			bcAssert(p_slot < m_states.size());

			return &m_states[p_slot];
		}

		template< typename TState, bcUINT TNum >
		TState* bc_pipeline_state_array_variable< TState, TNum >::get_first_slot()
		{
			return get_slot(0);
		}

		template< typename TState, bcUINT TNum >
		bcUINT bc_pipeline_state_array_variable< TState, TNum >::get_dirty_start() const
		{
			if (!update_needed())
				return 0;
			return m_dirty_start;
		}

		template< typename TState, bcUINT TNum >
		bcUINT bc_pipeline_state_array_variable< TState, TNum >::get_dirty_end() const
		{
			if (!update_needed())
				return 0;
			return m_dirty_end;
		}

		template< typename TState, bcUINT TNum >
		bcUINT bc_pipeline_state_array_variable< TState, TNum >::get_dirty_count() const
		{
			if (!update_needed()) 
				return 0;
			return get_dirty_end() - get_dirty_start() + 1;
		}

		template< typename TState, bcUINT TNum >
		bcUINT bc_pipeline_state_array_variable< TState, TNum >::variable_count() const
		{
			return TNum;
		}

		template< typename TState, bcUINT TNum >
		bool bc_pipeline_state_array_variable< TState, TNum >::update_needed() const
		{
			return m_update_needed;
		}

		template< typename TState, bcUINT TNum >
		void bc_pipeline_state_array_variable< TState, TNum >::reset_tracking()
		{
			m_update_needed = false;
			m_dirty_start = -1;
			m_dirty_end = -1;
		}
	}
}
