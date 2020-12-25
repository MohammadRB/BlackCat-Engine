// [03/20/2020 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Memory/bcPtr.h"

namespace black_cat
{
	namespace core
	{
		class bc_query_manager;
				
		class bc_query_context
		{
		public:
			virtual ~bc_query_context() = 0;

			template<class TQuery>
			TQuery& get_shared_query() const;
			
			bc_query_manager* m_query_manager{ nullptr };
			core_platform::bc_clock::update_param m_clock{ 0, 0 };

		protected:
			bc_query_context();
			
			bc_query_context(const bc_query_context&);

			bc_query_context& operator=(const bc_query_context&);
		};
				
		using bc_query_context_hash = bcSIZE;
		using bc_query_context_ptr = bc_unique_ptr<bc_query_context>;
		
		template<class T>
		bc_query_context_ptr bc_make_query_context(T&& p_context)
		{
			using t = std::decay_t<T>;
			return bc_make_unique<t>(bc_alloc_type::frame, std::forward<T>(p_context));
		}
		
		class bc_null_query_context : public bc_query_context
		{
		};
		
		inline bc_query_context::~bc_query_context() = default;

		inline bc_query_context::bc_query_context() = default;

		inline bc_query_context::bc_query_context(const bc_query_context&) = default;

		inline bc_query_context& bc_query_context::operator=(const bc_query_context&) = default;

		template< class TQuery >
		TQuery& bc_query_context::get_shared_query() const
		{
			return m_query_manager->_get_shared_query<TQuery>();
		}
	}
}