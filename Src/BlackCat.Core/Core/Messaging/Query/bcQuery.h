// [03/20/2020 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/bcExport.h"
#include "Core/Messaging/bcMessage.h"
#include "Core/Messaging/Query/bcQueryContext.h"

namespace black_cat
{
	namespace core
	{
		using bc_query_hash = bc_message_hash;
		
		class bc_iquery : public bc_imessage
		{
		public:
			virtual ~bc_iquery() = 0;

			virtual void execute(const bc_query_context& p_context) noexcept = 0;

		protected:
			bc_iquery(const bcCHAR* p_name);

			bc_iquery(const bc_iquery&);

			bc_iquery& operator=(const bc_iquery&);
		};

		/**
		 * \brief Do not use memory frame for internal allocation in queries
		 * \tparam TContext 
		 */
		template< class TContext >
		class bc_query : public bc_iquery
		{
		public:
			using context_t = TContext;
			
		public:
			virtual ~bc_query() = 0;

			void execute(const bc_query_context& p_context) noexcept override final;

			virtual void execute(const TContext& p_context) noexcept = 0;

		protected:
			bc_query(const bcCHAR* p_name);

			bc_query(const bc_query&);

			bc_query& operator=(const bc_query&);
		};

		inline bc_iquery::~bc_iquery() = default;

		inline bc_iquery::bc_iquery(const bcCHAR * p_name)
			: bc_imessage(p_name)
		{
		}

		inline bc_iquery::bc_iquery(const bc_iquery&) = default;

		inline bc_iquery& bc_iquery::operator=(const bc_iquery&) = default;
		
		template< class TContext >
		bc_query<TContext>::~bc_query() = default;

		template< class TContext >
		void bc_query<TContext>::execute(const bc_query_context& p_context) noexcept
		{
			execute(static_cast<const TContext&>(p_context));
		}

		template< class TContext >
		bc_query<TContext>::bc_query(const bcCHAR* p_name)
			: bc_iquery(p_name)
		{
		}

		template< class TContext >
		bc_query<TContext>::bc_query(const bc_query&) = default;

		template< class TContext >
		bc_query<TContext>& bc_query<TContext>::operator=(const bc_query&) = default;
	}
}