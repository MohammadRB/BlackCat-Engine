// [03/20/2020 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/bcExport.h"
#include "Core/bcConstant.h"
#include "Core/bcException.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcList.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "Core/Messaging/Query/bcQueryContext.h"
#include "Core/Messaging/Query/bcQueryProviderHandle.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcDelegate.h"
#include "Core/Utility/bcObjectPoolAllocator.h"
#include "Core/Utility/bcObjectPool.h"
#include "Core/Utility/bcObjectStackPool.h"

namespace black_cat
{
	namespace core
	{
		template<typename T>
		class bc_task;

		class BC_CORE_DLL bc_query_manager final : public bci_service
		{
			BC_SERVICE(qr_mng)

		public:
			using provider_delegate_t = bc_delegate<bc_query_context_ptr()>;

		private:
			class _provider_entry;
			class _query_entry;

			using provider_map_t = bc_unordered_map_program<bc_query_context_hash, _provider_entry>;
			using query_list_t = bc_list<_query_entry, bc_object_pool_allocator<_query_entry>>;

		public:
			bc_query_manager();

			bc_query_manager(bc_query_manager&&) noexcept = delete;

			~bc_query_manager() override;

			bc_query_manager& operator=(bc_query_manager&&) noexcept = delete;

			/**
			 * \brief Context can be allocated using frame memory
			 * \tparam TContext
			 * \param p_delegate
			 * \return
			 */
			template<class TContext>
			bc_query_provider_handle register_query_provider(provider_delegate_t&& p_delegate);

			void replace_query_provider(const bc_query_provider_handle& p_provider_handle, provider_delegate_t&& p_delegate);

			void unregister_query_provider(const bc_query_provider_handle& p_provider_handle);

			/**
			 * \brief Put query into queue to be executed.
			 * \n The ownership of query is not moved and the caller is responsible for query object lifetime.
			 * \tparam TQuery 
			 * \param p_query 
			 * \return 
			 */
			template<class TQuery>
			bc_query_result<bci_query> queue_ext_query(TQuery& p_query);

			/**
			 * \brief Put query into queue to be executed.
			 * \n The ownership of query is moved.
			 * \tparam TQuery 
			 * \param p_query 
			 * \return 
			 */
			template<class TQuery>
			bc_query_result<std::decay_t<TQuery>> queue_query(TQuery p_query);

			template<class TQuery>
			void queue_shared_ext_query(TQuery& p_query);

			template<class TQuery>
			void queue_shared_query(TQuery p_query);

			bcUINT32 process_query_queue(const platform::bc_clock::update_param& p_clock);

			bc_task<bcUINT32> process_query_queue_async(const platform::bc_clock::update_param& p_clock);

			void clear_queries();
			
			void swap_frame();

			template<class TQuery>
			TQuery& _get_shared_query();

			void _mark_shared_state(_bc_query_shared_state& p_shared_state);

		private:
			bc_query_provider_handle _register_query_provider(bc_query_context_hash p_context_hash, provider_delegate_t&& p_delegate);

			_bc_query_shared_state& _queue_query(bc_query_context_hash p_context_hash, bool p_is_shared, bool p_has_ownership, bci_query* p_query);
			
			platform::bc_shared_mutex m_providers_lock;
			provider_map_t m_providers;

			platform::bc_mutex m_executed_queries_lock;
			bc_concurrent_memory_pool m_query_entry_pool;
			query_list_t m_executed_shared_queries;
			query_list_t m_executed_queries;
		};

		class bc_query_manager::_query_entry : public _bc_query_shared_state
		{
		public:
			_query_entry(_provider_entry& p_provider, bool p_is_shared, bool p_has_ownership, bci_query* p_query);

			_query_entry(_query_entry&&) noexcept;

			~_query_entry();

			_query_entry& operator=(_query_entry&&) noexcept;
			
			_provider_entry* m_provider;
			query_list_t::iterator m_iterator;
			bool m_is_shared;
			bool m_has_ownership;
		};

		class bc_query_manager::_provider_entry
		{
		public:
			explicit _provider_entry(bc_concurrent_memory_pool& p_memory_pool);

			_provider_entry(_provider_entry&&) noexcept;

			~_provider_entry();

			_provider_entry& operator=(_provider_entry&&) noexcept;

			provider_delegate_t m_provider_delegate;
			bc_query_context_ptr m_provided_context;
			platform::bc_spin_mutex m_queries_lock;
			query_list_t m_queries;
		};

		template<class TContext>
		bc_query_provider_handle bc_query_manager::register_query_provider(provider_delegate_t&& p_delegate)
		{
			static_assert(std::is_base_of_v<bc_query_context, TContext>, "TContext must be derived from bc_query_context");

			auto& l_context_type_info = typeid(TContext);
			return _register_query_provider(l_context_type_info.hash_code(), std::move(p_delegate));
		}

		template<class TQuery>
		bc_query_result<bci_query> bc_query_manager::queue_ext_query(TQuery& p_query)
		{
			using query_t = std::decay_t<TQuery>;
			static_assert(std::is_base_of_v<bci_query, query_t>, "TQuery must be derived from bc_iquery");
			static_assert(!query_t::is_shared(), "TQuery must not be a shared query");

			auto& l_context_type_info = typeid(typename query_t::context_t);

			auto& l_shared_state = _queue_query(l_context_type_info.hash_code(), false, false, &p_query);

			return bc_query_result<bci_query>(*this, l_shared_state);
		}

		template<class TQuery>
		bc_query_result<std::decay_t<TQuery>> bc_query_manager::queue_query(TQuery p_query)
		{
			using query_t = std::decay_t<TQuery>;
			static_assert(std::is_base_of_v<bci_query, query_t>, "TQuery must be derived from bc_iquery");
			static_assert(!query_t::is_shared(), "TQuery must not be a shared query");
			
			auto* l_query = BC_NEW(query_t(std::move(p_query)), core::bc_alloc_type::unknown);
			auto& l_context_type_info = typeid(typename query_t::context_t);
			
			auto& l_shared_state = _queue_query(l_context_type_info.hash_code(), false, true, l_query);

			return bc_query_result<query_t>(*this, l_shared_state);
		}

		template<class TQuery>
		void bc_query_manager::queue_shared_ext_query(TQuery& p_query)
		{
			using query_t = std::decay_t<TQuery>;
			static_assert(std::is_base_of_v<bci_query, query_t>, "TQuery must be derived from bc_iquery");
			static_assert(query_t::is_shared(), "TQuery must be a shared query");
			
			auto& l_context_type_info = typeid(typename query_t::context_t);
			
			auto& l_shared_state = _queue_query(l_context_type_info.hash_code(), true, false, &p_query);
		}

		template<class TQuery>
		void bc_query_manager::queue_shared_query(TQuery p_query)
		{
			using query_t = std::decay_t<TQuery>;
			static_assert(std::is_base_of_v<bci_query, query_t>, "TQuery must be derived from bc_iquery");
			static_assert(query_t::is_shared(), "TQuery must be a shared query");
			
			auto* l_query = BC_NEW(query_t(std::forward<query_t>(p_query)), core::bc_alloc_type::unknown);
			auto& l_context_type_info = typeid(typename query_t::context_t);
			
			auto& l_shared_state = _queue_query(l_context_type_info.hash_code(), true, true, l_query);
		}

		template<class TQuery>
		TQuery& bc_query_manager::_get_shared_query()
		{
			static_assert(std::is_base_of_v<bci_query, TQuery>, "TQuery must be derived from bc_iquery");
			static_assert(TQuery::is_shared(), "TQuery must be a shared query");

			for (const auto& l_shared_query : m_executed_shared_queries)
			{
				if (bci_message::is<TQuery>(*l_shared_query.m_query))
				{
					// use acquire memory order so memory changes become available for calling thread
					l_shared_query.m_state.load(platform::bc_memory_order::acquire);
					return static_cast<TQuery&>(*l_shared_query.m_query);
				}
			}

			throw bc_invalid_operation_exception("No shared query were found");
		}

		template<class TQuery>
		TQuery& bc_query_context::get_shared_query() const
		{
			// query manager definition is available here
			return m_query_manager->_get_shared_query<TQuery>();
		}
	}
}