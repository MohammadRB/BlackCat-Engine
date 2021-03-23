// [10/02/2016 MRB]

#pragma once

#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/Utility/bcNoCopy.h"

namespace black_cat
{
	namespace platform
	{
		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_context;
		using bc_script_context = bc_platform_script_context< core_platform::g_current_platform >;

		template< core_platform::bc_platform TPlatform >
		class bc_script_context_scope : public core_platform::bc_no_copy
		{
		public:
			bc_script_context_scope(bc_script_context& p_context) noexcept
				: m_context(p_context),
				m_prev_context(*m_context.m_runtime->get_active_context())
			{
				m_context.m_runtime->set_active_context(&p_context);
			}

			~bc_script_context_scope()
			{
				m_context.m_runtime->set_active_context(&m_prev_context);
			}

			bc_script_context& get_context() const noexcept
			{
				return m_context;
			}

		private:
			bc_script_context& m_context;
			bc_script_context& m_prev_context;
		};
	}
}