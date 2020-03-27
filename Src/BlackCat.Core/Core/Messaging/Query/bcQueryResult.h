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
			explicit _bc_query_shared_state(bc_iquery& p_query);

			_bc_query_shared_state(_bc_query_shared_state&&) noexcept;

			~_bc_query_shared_state();

			_bc_query_shared_state& operator=(_bc_query_shared_state&&) noexcept;
			
			core_platform::bc_atomic< bool > m_state_flag;
			bc_iquery& m_query;
		};

		inline _bc_query_shared_state::_bc_query_shared_state(bc_iquery& p_query)
			: m_state_flag(false),
			m_query(p_query)
		{
		}

		inline _bc_query_shared_state::_bc_query_shared_state(_bc_query_shared_state&& p_other) noexcept
			: m_state_flag(p_other.m_state_flag.load(core_platform::bc_memory_order::relaxed)),
			m_query(p_other.m_query)
		{
		}

		inline _bc_query_shared_state::~_bc_query_shared_state() = default;

		inline _bc_query_shared_state& _bc_query_shared_state::operator=(_bc_query_shared_state&& p_other) noexcept
		{
			m_state_flag.store(p_other.m_state_flag.load(core_platform::bc_memory_order::relaxed), core_platform::bc_memory_order::relaxed);
						
			return *this;
		}

		template< class TQuery >
		class bc_query_result
		{
			friend class bc_query_manager;
			
		public:
			bc_query_result(bc_query_result&&) noexcept;

			~bc_query_result();

			bc_query_result& operator=(bc_query_result&&) noexcept;

			bool is_executed() const noexcept;

			/**
			 * \brief Before calling this function availability of query must be examined by <b>is_executed</b> function.
			 * \return Original query which was scheduled.
			 */
			TQuery get() const noexcept(false);

		private:
			explicit bc_query_result(bc_query_manager& p_query_manager, const _bc_query_shared_state& p_shared_state);

			bc_query_manager& m_query_manager;
			const _bc_query_shared_state& m_shared_state;
		};

		template< class TQuery >
		bc_query_result<TQuery>::bc_query_result(bc_query_manager& p_query_manager, const _bc_query_shared_state& p_shared_state)
			: m_query_manager(p_query_manager),
			m_shared_state(p_shared_state)
		{
		}

		template< class TQuery >
		bc_query_result<TQuery>::bc_query_result(bc_query_result&&) noexcept = default;

		template< class TQuery >
		bc_query_result<TQuery>::~bc_query_result() = default;

		template< class TQuery >
		bc_query_result<TQuery>& bc_query_result<TQuery>::operator=(bc_query_result&&) noexcept = default;

		template< class TQuery >
		bool bc_query_result<TQuery>::is_executed() const noexcept
		{
			// use acquire memory order so memory changes become available for calling thread
			return m_shared_state.m_state_flag.load(core_platform::bc_memory_order::acquire);
		}

		template< class TQuery >
		TQuery bc_query_result<TQuery>::get() const noexcept(false)
		{
			const bool l_is_executed = is_executed();

			if(!l_is_executed)
			{
				bcAssert(false);
				throw bc_invalid_operation_exception("Query is not ready.");
			}

			TQuery l_query = std::move(*static_cast<TQuery*>(m_shared_state.m_query));

			m_query_manager._mark_shared_state(m_shared_state);
			
			return l_query;
		}
	}
}