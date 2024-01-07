// [23/03/2020 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Core/Utility/bcTemplateMetaType.h"
#include "Core/bcExport.h"
#include "Core/bcException.h"

namespace black_cat::core
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
			
		platform::bc_atomic<state> m_state;
		bci_query* m_query;
	};

	inline _bc_query_shared_state::_bc_query_shared_state(bci_query& p_query)
		: m_state(state::queued),
		  m_query(&p_query)
	{
	}

	inline _bc_query_shared_state::_bc_query_shared_state(_bc_query_shared_state&& p_other) noexcept
		: m_state(p_other.m_state.load(platform::bc_memory_order::relaxed)),
		  m_query(p_other.m_query)
	{
		p_other.m_query = nullptr;
	}

	inline _bc_query_shared_state::~_bc_query_shared_state() = default;

	inline _bc_query_shared_state& _bc_query_shared_state::operator=(_bc_query_shared_state&& p_other) noexcept
	{
		m_state.store(p_other.m_state.load(platform::bc_memory_order::relaxed), platform::bc_memory_order::relaxed);
		m_query = p_other.m_query;
		p_other.m_query = nullptr;
						
		return *this;
	}
	
	class BC_CORE_DLL bc_query_result
	{
		friend class bc_query_manager;

	public:
		bc_query_result() noexcept;

		bc_query_result(bc_query_result&&) noexcept;

		~bc_query_result();

		bc_query_result& operator=(bc_query_result&&) noexcept;

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
		bci_query& get();

		template<class TQuery>
		TQuery& get();

	protected:
		bc_query_result(bc_query_manager& p_query_manager, _bc_query_shared_state& p_shared_state) noexcept;

	private:
		bc_query_manager* m_query_manager;
		_bc_query_shared_state* m_shared_state;
	};

	template <class TQuery>
	TQuery& bc_query_result::get()
	{
		using query_t = std::decay_t<TQuery>;
		static_assert(std::is_base_of_v<bci_query, query_t>, "TQuery must be derived from bc_iquery");

		return static_cast<TQuery&>(get());
	}

	template<class TQuery>
	class bc_query_result_t : public bc_query_result
	{
		friend class bc_query_manager;
			
	public:
		bc_query_result_t() noexcept;
						
		bc_query_result_t(bc_query_result_t&&) noexcept;

		template<class TQuery1>
		bc_query_result_t(bc_query_result_t<TQuery1>&& p_other) noexcept;

		~bc_query_result_t();

		bc_query_result_t& operator=(bc_query_result_t&&) noexcept;

		template<class TQuery1>
		bc_query_result_t& operator=(bc_query_result_t<TQuery1>&& p_other) noexcept;
		
		/**
		 * \brief Before calling this function availability of query must be examined by <b>is_executed</b> function.
		 * \n If query is not executed calling this function will throw an exception.
		 * \n After calling this function the internal state of query will be removed and any subsequent call on this object will be undefined.
		 * \return Original query which was scheduled.
		 */
		TQuery get();
		
	private:
		bc_query_result_t(bc_query_manager& p_query_manager, _bc_query_shared_state& p_shared_state) noexcept;
	};

	template<class TQuery>
	bc_query_result_t<TQuery>::bc_query_result_t(bc_query_manager& p_query_manager, _bc_query_shared_state& p_shared_state) noexcept
		: bc_query_result(p_query_manager, p_shared_state)
	{
	}
		
	template<class TQuery>
	bc_query_result_t<TQuery>::bc_query_result_t() noexcept = default;

	template<class TQuery>
	bc_query_result_t<TQuery>::bc_query_result_t(bc_query_result_t&& p_other) noexcept = default;

	template<class TQuery>
	template<class TQuery1>
	bc_query_result_t<TQuery>::bc_query_result_t(bc_query_result_t<TQuery1>&& p_other) noexcept
	{
		operator=(std::move(p_other));
	}

	template<class TQuery>
	bc_query_result_t<TQuery>::~bc_query_result_t() = default;

	template<class TQuery>
	bc_query_result_t<TQuery>& bc_query_result_t<TQuery>::operator=(bc_query_result_t&& p_other) noexcept = default;

	template<class TQuery>
	template<class TQuery1>
	bc_query_result_t<TQuery>& bc_query_result_t<TQuery>::operator=(bc_query_result_t<TQuery1>&& p_other) noexcept
	{
		static_assert(std::is_base_of_v<TQuery, TQuery1>, "TQuery1 must be a derived type of TQuery");
		bc_query_result::operator=(std::move(p_other));

		return *this;
	}

	template<class TQuery>
	TQuery bc_query_result_t<TQuery>::get()
	{
		TQuery l_query = std::move(bc_query_result::get<TQuery>());
		return l_query;
	}
}
