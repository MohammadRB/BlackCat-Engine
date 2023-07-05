// [11/10/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/bcPlatform.h"

namespace black_cat::platform
{
	template<bc_platform TPlatform>
	struct bc_platform_condition_variable_pack
	{
			
	};

	template<bc_platform TPlatform>
	class bc_platform_condition_variable : private bc_no_copy
	{
	public:
		using platform_pack = bc_platform_condition_variable_pack<TPlatform>;

	public:
		bc_platform_condition_variable();

		~bc_platform_condition_variable();

		void notify_one();

		void notify_all() noexcept(true);

		template< typename Lockable >
		void wait(Lockable& p_lock);

		template < typename Lockable, typename Predicate >
		void wait(Lockable& p_lock, Predicate p_pred);

		template < typename Lockable >
		bool wait_for(Lockable& p_lock, bcUINT64 p_nano);

		template < typename Lockable, typename Predicate >
		bool wait_for(Lockable& p_lock, bcUINT64 p_nano, Predicate p_pred);

	private:
		platform_pack m_pack;
	};

	using bc_condition_variable = bc_platform_condition_variable<g_current_platform>;
}
