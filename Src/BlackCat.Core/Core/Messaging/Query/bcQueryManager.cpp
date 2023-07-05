// [22/03/2020 MRB]

#include "Core/CorePCH.h"
#include "Core/Concurrency/bcConcurrency.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/bcException.h"

namespace black_cat::core
{
	bc_query_manager::bc_query_manager()
		: m_executed_shared_queries(m_query_entry_pool),
		  m_executed_queries(m_query_entry_pool)
	{
		m_query_entry_pool.initialize(1000, sizeof(query_list_t::node_type), bc_alloc_type::unknown);
	}

	bc_query_manager::~bc_query_manager() = default;

	void bc_query_manager::replace_query_provider(const bc_query_provider_handle& p_provider_handle, provider_delegate_t&& p_delegate)
	{
		{
			platform::bc_lock_guard l_guard(m_providers_lock);

			const auto l_provider_entry = m_providers.find(p_provider_handle.m_context_hash);
			BC_ASSERT(l_provider_entry != std::end(m_providers));

			l_provider_entry->second.m_provider_delegate = std::move(p_delegate);
		}
	}

	void bc_query_manager::unregister_query_provider(const bc_query_provider_handle& p_provider_handle)
	{
		{
			platform::bc_lock_guard l_guard(m_providers_lock);

			m_providers.erase(p_provider_handle.m_context_hash);
		}
	}
		
	bcUINT bc_query_manager::process_query_queue(const platform::bc_clock::update_param& p_clock)
	{
		{
			platform::bc_shared_lock l_providers_guard(m_providers_lock);
			platform::bc_lock_guard l_executed_queries_guard(m_executed_queries_lock);

			const query_list_t::iterator l_last_exist_shared_query = std::rbegin(m_executed_shared_queries).base();
			const query_list_t::iterator l_last_exist_query = std::rbegin(m_executed_queries).base();

			for (auto& l_provider : m_providers)
			{
				if (!l_provider.second.m_provided_context)
				{
					l_provider.second.m_provided_context = l_provider.second.m_provider_delegate();
					l_provider.second.m_provided_context->m_query_manager = this;
					l_provider.second.m_provided_context->m_clock = p_clock;
				}

				const query_list_t::iterator l_local_last_exist_query = std::rbegin(m_executed_queries).base();

				{
					platform::bc_lock_guard l_queries_guard(l_provider.second.m_queries_lock);

					if (l_provider.second.m_queries.empty())
					{
						continue;
					}

					m_executed_queries.splice
					(
						std::end(m_executed_queries),
						l_provider.second.m_queries
					);
				}

				auto l_first_inserted_query = l_local_last_exist_query != std::end(m_executed_queries)
					                              ? ++static_cast<query_list_t::iterator>(l_local_last_exist_query)
					                              : std::begin(m_executed_queries);

				while (l_first_inserted_query != std::end(m_executed_queries))
				{
					if (l_first_inserted_query->m_is_shared)
					{
						auto l_next = l_first_inserted_query;
						++l_next;

						m_executed_shared_queries.splice
						(
							std::end(m_executed_shared_queries),
							m_executed_queries,
							l_first_inserted_query
						);

						l_first_inserted_query = l_next;
						continue;
					}

					const auto l_query_state = l_first_inserted_query->m_state.load(platform::bc_memory_order::relaxed);
					if (l_query_state == _bc_query_shared_state::state::deleted)
					{
						l_first_inserted_query = m_executed_queries.erase(l_first_inserted_query);
						continue;
					}

					l_first_inserted_query->m_iterator = l_first_inserted_query;
					l_first_inserted_query->m_state.store(_bc_query_shared_state::state::activated, platform::bc_memory_order::relaxed);
					++l_first_inserted_query;
				}
			}

			const auto l_first_inserted_shared_query = l_last_exist_shared_query != std::end(m_executed_shared_queries)
				                                           ? ++static_cast<query_list_t::iterator>(l_last_exist_shared_query)
				                                           : std::begin(m_executed_shared_queries);
			const auto l_first_inserted_query = l_last_exist_query != std::end(m_executed_queries)
				                                    ? ++static_cast<query_list_t::iterator>(l_last_exist_query)
				                                    : std::begin(m_executed_queries);

			const auto l_num_shared_queries = std::distance(l_first_inserted_shared_query, std::end(m_executed_shared_queries));
			const auto l_num_queries = std::distance(l_first_inserted_query, std::end(m_executed_queries));
			const auto l_num_shared_threads = std::min(bc_concurrency::hardware_worker_count(), static_cast<bcUINT>(l_num_shared_queries));
			const auto l_num_thread = std::min(bc_concurrency::hardware_worker_count(), std::max(static_cast<bcUINT>(l_num_queries) / 2U, 1U));

			bc_concurrency::concurrent_for_each
			(
				l_num_shared_threads,
				l_first_inserted_shared_query,
				std::end(m_executed_shared_queries),
				[&](_query_entry& p_query)
				{
					BC_ASSERT(p_query.m_state.load(platform::bc_memory_order::relaxed) != _bc_query_shared_state::state::executed);

					p_query.m_query->execute(*p_query.m_provider->m_provided_context);
					// use release memory order so memory changes become available for calling thread
					p_query.m_state.store(_bc_query_shared_state::state::executed, platform::bc_memory_order::release);
				}
			);

			bc_concurrency::concurrent_for_each
			(
				l_num_thread,
				l_first_inserted_query,
				std::end(m_executed_queries),
				[&](_query_entry& p_query)
				{
					BC_ASSERT(p_query.m_state.load(platform::bc_memory_order::relaxed) != _bc_query_shared_state::state::executed);

					p_query.m_query->execute(*p_query.m_provider->m_provided_context);
					// use release memory order so memory changes become available for calling thread
					p_query.m_state.store(_bc_query_shared_state::state::executed, platform::bc_memory_order::release);
				}
			);

			return l_num_shared_queries + l_num_queries;
		}
	}

	bc_task<bcUINT> bc_query_manager::process_query_queue_async(const platform::bc_clock::update_param& p_clock)
	{
		auto l_task = bc_concurrency::start_task
		(
			bc_delegate<bcUINT()>
			(
				[=, &p_clock]()
				{
					return process_query_queue(p_clock);
				}
			)
		);

		return l_task;
	}

	void bc_query_manager::clear_queries()
	{
		for(auto& l_provider : m_providers)
		{
			l_provider.second.m_queries.clear();
		}
		m_executed_queries.clear();
		m_executed_shared_queries.clear();
	}

	void bc_query_manager::swap_frame()
	{
		for (auto& l_provider : m_providers)
		{
			// Free context to make it possible to use memory_frame in context allocation
			l_provider.second.m_provided_context.reset();
		}

		m_executed_queries.remove_if([=](const _query_entry& p_query)
		{
			return p_query.m_state.load(platform::bc_memory_order::relaxed) == _bc_query_shared_state::state::deleted;
		});
		m_executed_shared_queries.clear();
	}

	void bc_query_manager::_mark_shared_state(_bc_query_shared_state& p_shared_state)
	{
		p_shared_state.m_state.store(_bc_query_shared_state::state::deleted, platform::bc_memory_order::relaxed);
	}

	bc_query_provider_handle bc_query_manager::_register_query_provider(bc_query_context_hash p_context_hash, provider_delegate_t&& p_delegate)
	{
		{
			platform::bc_shared_mutex_guard l_guard(m_providers_lock);

			auto l_provider_entry = m_providers.find(p_context_hash);
			if (l_provider_entry == std::end(m_providers))
			{
				l_provider_entry = m_providers.insert(provider_map_t::value_type(p_context_hash, _provider_entry(m_query_entry_pool))).first;
			}

			if (l_provider_entry->second.m_provider_delegate.is_valid())
			{
				throw bc_invalid_operation_exception("A provider has already been added with a same hash");
			}

			l_provider_entry->second.m_provider_delegate = std::move(p_delegate);
		}

		return bc_query_provider_handle(p_context_hash);
	}
		
	_bc_query_shared_state& bc_query_manager::_queue_query(bc_query_context_hash p_context_hash, bool p_is_shared, bool p_has_ownership, bci_query* p_query)
	{
		{
			platform::bc_shared_lock l_providers_guard(m_providers_lock);

			const auto l_provider_entry = m_providers.find(p_context_hash);
			BC_ASSERT(l_provider_entry != std::end(m_providers));

			auto& l_provider = l_provider_entry->second;

			{
				platform::bc_lock_guard l_queries_guard(l_provider.m_queries_lock);

				l_provider.m_queries.push_back(_query_entry(l_provider, p_is_shared, p_has_ownership, p_query));

				return l_provider.m_queries.back();
			}
		}
	}

	bc_query_manager::_query_entry::_query_entry(_provider_entry& p_provider, bool p_is_shared, bool p_has_ownership, bci_query* p_query)
		: _bc_query_shared_state(*p_query),
		  m_provider(&p_provider),
		  m_iterator(std::end(p_provider.m_queries)),
		  m_is_shared(p_is_shared),
		  m_has_ownership(p_has_ownership)
	{
	}

	bc_query_manager::_query_entry::_query_entry(_query_entry&& p_other) noexcept
		: _bc_query_shared_state(std::move(p_other)),
		  m_provider(p_other.m_provider),
		  m_iterator(p_other.m_iterator),
		  m_is_shared(p_other.m_is_shared),
		  m_has_ownership(p_other.m_has_ownership)
	{
	}

	bc_query_manager::_query_entry::~_query_entry()
	{
		if(m_has_ownership && m_query)
		{
			BC_DELETE(m_query);
		}
	}

	bc_query_manager::_query_entry& bc_query_manager::_query_entry::operator=(_query_entry&& p_other) noexcept
	{
		_bc_query_shared_state::operator=(std::move(p_other));
		m_provider = p_other.m_provider;
		m_iterator = p_other.m_iterator;
		m_is_shared = p_other.m_is_shared;
		m_has_ownership = p_other.m_has_ownership;

		return *this;
	}

	bc_query_manager::_provider_entry::_provider_entry(bc_concurrent_memory_pool& p_memory_pool)
		: m_queries(query_list_t::allocator_type(p_memory_pool))
	{
	}

	bc_query_manager::_provider_entry::_provider_entry(_provider_entry&& p_other) noexcept
		: m_provider_delegate(std::move(p_other.m_provider_delegate)),
		  m_provided_context(std::move(p_other.m_provided_context)),
		  m_queries(std::move(p_other.m_queries))
	{
	}

	bc_query_manager::_provider_entry::~_provider_entry() = default;

	bc_query_manager::_provider_entry& bc_query_manager::_provider_entry::operator=(_provider_entry&& p_other) noexcept
	{
		m_provider_delegate = std::move(p_other.m_provider_delegate);
		m_provided_context = std::move(p_other.m_provided_context);
		m_queries = std::move(p_other.m_queries);

		return *this;
	}
}
