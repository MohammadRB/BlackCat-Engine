// [03/20/2020 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Messaging/bcMessage.h"
#include "Core/Messaging/Query/bcQueryContext.h"
#include "Core/bcConstant.h"

namespace black_cat
{
	namespace core
	{
		class bc_query_manager;
		using bc_query_hash = bc_message_hash;
		
		class bci_query : public bci_message
		{
			friend class bc_query_manager;
			
		public:
			virtual ~bci_query() = 0;

		protected:
			bci_query(const bcCHAR* p_name);

			bci_query(const bci_query&);

			bci_query& operator=(const bci_query&);

			virtual void execute(const bc_query_context& p_context) noexcept = 0;
		};

		/**
		 * \brief Do not use memory frame for internal allocation in queries
		 * \tparam TContext 
		 */
		template< class TContext >
		class bc_query : public bci_query
		{
			friend class bc_query_manager;
			
		public:
			using context_t = TContext;
			
		public:
			virtual ~bc_query() = 0;

		protected:
			bc_query(const bcCHAR* p_name);

			bc_query(const bc_query&);

			bc_query& operator=(const bc_query&);

			void execute(const bc_query_context& p_context) noexcept override final;

			virtual void execute(const TContext& p_context) noexcept = 0;
		};

		inline bci_query::~bci_query() = default;

		inline bci_query::bci_query(const bcCHAR * p_name)
			: bci_message(p_name)
		{
		}

		inline bci_query::bci_query(const bci_query&) = default;

		inline bci_query& bci_query::operator=(const bci_query&) = default;
		
		template< class TContext >
		bc_query<TContext>::~bc_query() = default;

		template< class TContext >
		void bc_query<TContext>::execute(const bc_query_context& p_context) noexcept
		{
			execute(static_cast<const TContext&>(p_context));
		}

		template< class TContext >
		bc_query<TContext>::bc_query(const bcCHAR* p_name)
			: bci_query(p_name)
		{
		}

		template< class TContext >
		bc_query<TContext>::bc_query(const bc_query&) = default;

		template< class TContext >
		bc_query<TContext>& bc_query<TContext>::operator=(const bc_query&) = default;
	}
}