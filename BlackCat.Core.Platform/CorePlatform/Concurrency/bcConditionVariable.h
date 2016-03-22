// [10/11/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/bcPlatform.h"

namespace black_cat
{
	namespace core_platform
	{
		template<bc_platform TPlatform>
		struct bc_platform_condition_variable_pack
		{
			
		};

		template< bc_platform TPlatform >
		class bc_platform_condition_variable : private bc_no_copy
		{
		public:
			using platform_pack = bc_platform_condition_variable_pack<TPlatform>;

		public:
			bc_platform_condition_variable();

			~bc_platform_condition_variable();

			bcInline void notify_one();

			bcInline void notify_all() noexcept(true);

			template< typename Lockable >
			bcInline void wait(Lockable& p_lock);

			template < typename Lockable, typename Predicate >
			bcInline void wait(Lockable& p_lock, Predicate p_pred);

			template < typename Lockable >
			bcInline bool wait_for(Lockable& p_lock, const bcUINT64 p_nano);

			template < typename Lockable, typename Predicate >
			bcInline bool wait_for(Lockable& p_lock, const bcUINT64 p_nano, Predicate p_pred);

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_condition_variable = bc_platform_condition_variable< g_current_platform >;
	}
}