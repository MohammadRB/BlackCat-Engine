// [03/23/2020 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/bcException.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Core/Utility/bcTemplateMetaType.h"

namespace black_cat
{
	namespace core
	{
		class bc_query_manager;

		class _bc_query_shared_state
		{
		public:
			enum class state
			{
				queued, activated, executed, deleted
			};
			
		public:
			explicit _bc_query_shared_state(bci_query& p_query);

			_bc_query_shared_state(_bc_query_shared_state&&) noexcept;

			~_bc_query_shared_state();

			_bc_query_shared_state& operator=(_bc_query_shared_state&&) noexcept;
			
			core_platform::bc_atomic<state> m_state;
			bci_query* m_query;
		};

		inline _bc_query_shared_state::_bc_query_shared_state(bci_query& p_query)
			: m_state(state::queued),
			m_query(&p_query)
		{
		}

		inline _bc_query_shared_state::_bc_query_shared_state(_bc_query_shared_state&& p_other) noexcept
			: m_state(p_other.m_state.load(core_platform::bc_memory_order::relaxed)),
			m_query(p_other.m_query)
		{
			p_other.m_query = nullptr;
		}

		inline _bc_query_shared_state::~_bc_query_shared_state() = default;

		inline _bc_query_shared_state& _bc_query_shared_state::operator=(_bc_query_shared_state&& p_other) noexcept
		{
			m_state.store(p_other.m_state.load(core_platform::bc_memory_order::relaxed), core_platform::bc_memory_order::relaxed);
			m_query = p_other.m_query;
			p_other.m_query = nullptr;
						
			return *this;
		}

		template<class TQuery>
		class bc_query_result
		{
			friend class bc_query_manager;
			
		public:
			bc_query_result() noexcept;
						
			bc_query_result(bc_query_result&&) noexcept;

			template<class TQuery1>
			bc_query_result(bc_query_result<TQuery1>&& p_other) noexcept;

			~bc_query_result();

			bc_query_result& operator=(bc_query_result&&) noexcept;

			template<class TQuery1>
			bc_query_result& operator=(bc_query_result<TQuery1>&& p_other) noexcept;

			/**
			 * \brief Check to see if query is executed and the result is ready.
			 * \n After the result is fetched the internal state of query will be removed and any subsequent call on this object will be undefined.
			 * \return 
			 */
			bool is_executed() const noexcept;

			/**
			 * \brief Before calling this function availability of query must be examined by <b>is_executed</b> function.
			 * \n If query is not executed calling this function will throw an exception.
			 * \n After calling this function the internal state of query will be removed and any subsequent call on this object will be undefined.
			 * \return Original query which was scheduled.
			 */
			TQuery get();

		private:
			bc_query_result(bc_query_manager& p_query_manager, _bc_query_shared_state& p_shared_state) noexcept;

			bc_query_manager* m_query_manager;
			_bc_query_shared_state* m_shared_state;
		};

		template<class TQuery>
		bc_query_result<TQuery>::bc_query_result(bc_query_manager& p_query_manager, _bc_query_shared_state& p_shared_state) noexcept
			: m_query_manager(&p_query_manager),
			m_shared_state(&p_shared_state)
		{
		}

		template<class TQuery>
		bc_query_result<TQuery>::bc_query_result() noexcept
			: m_query_manager(nullptr),
			m_shared_state(nullptr)
		{
		}

		template<class TQuery>
		bc_query_result<TQuery>::bc_query_result(bc_query_result&& p_other) noexcept
			: m_query_manager(nullptr),
			m_shared_state(nullptr)
		{
			operator=(std::move(p_other));
		}

		template<class TQuery>
		template<class TQuery1>
		bc_query_result<TQuery>::bc_query_result(bc_query_result<TQuery1>&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		template<class TQuery>
		bc_query_result<TQuery>::~bc_query_result()
		{
			if(m_shared_state)
			{
				m_query_manager->_mark_shared_state(*m_shared_state);
			}
		}

		template<class TQuery>
		bc_query_result<TQuery>& bc_query_result<TQuery>::operator=(bc_query_result&& p_other) noexcept
		{
			if(m_shared_state)
			{
				m_query_manager->_mark_shared_state(*m_shared_state);
			}
			
			m_query_manager = p_other.m_query_manager;
			m_shared_state = p_other.m_shared_state;
			p_other.m_query_manager = nullptr;
			p_other.m_shared_state = nullptr;

			return *this;
		}

		template<class TQuery>
		template<class TQuery1>
		bc_query_result<TQuery>& bc_query_result<TQuery>::operator=(bc_query_result<TQuery1>&& p_other) noexcept
		{
			static_assert(std::is_base_of_v<TQuery, TQuery1>, "TQuery1 must be a derived type of TQuery");

			if (m_shared_state)
			{
				m_query_manager->_mark_shared_state(*m_shared_state);
			}
			
			m_query_manager = p_other.m_query_manager;
			m_shared_state = p_other.m_shared_state;
			p_other.m_query_manager = nullptr;
			p_other.m_shared_state = nullptr;

			return *this;
		}

		template<class TQuery>
		bool bc_query_result<TQuery>::is_executed() const noexcept
		{
			// use acquire memory order so memory changes become available for calling thread
			return m_shared_state && m_shared_state->m_state.load(core_platform::bc_memory_order::acquire) == _bc_query_shared_state::state::executed;
		}

		template<class TQuery>
		TQuery bc_query_result<TQuery>::get()
		{
			const bool l_is_executed = is_executed();

			if(!l_is_executed)
			{
				BC_ASSERT(false);
				throw bc_invalid_operation_exception("Query is not ready.");
			}

			TQuery l_query = std::move(*static_cast<TQuery*>(m_shared_state->m_query));

			m_query_manager->_mark_shared_state(*m_shared_state);
			m_shared_state = nullptr;
			
			return l_query;
		}
	}
}