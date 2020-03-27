// [03/20/2020 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Memory/bcPtr.h"

namespace black_cat
{
	namespace core
	{
		using bc_query_context_hash = bcSIZE;
		
		class bc_query_context
		{
		public:
			virtual ~bc_query_context() = 0;

			core_platform::bc_clock::update_param m_clock;

		protected:
			explicit bc_query_context(const core_platform::bc_clock::update_param& p_clock);
			
			bc_query_context(const bc_query_context&);

			bc_query_context& operator=(const bc_query_context&);
		};

		using bc_query_context_ptr = bc_unique_ptr<bc_query_context>;
		
		inline bc_query_context::~bc_query_context() = default;

		inline bc_query_context::bc_query_context(const core_platform::bc_clock::update_param& p_clock)
			: m_clock(p_clock)
		{
		}

		inline bc_query_context::bc_query_context(const bc_query_context&) = default;

		inline bc_query_context& bc_query_context::operator=(const bc_query_context&) = default;
	}
}