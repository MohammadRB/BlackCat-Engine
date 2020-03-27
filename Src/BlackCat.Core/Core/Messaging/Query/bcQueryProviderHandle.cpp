// [03/21/2020 MRB]

#include "Core/CorePCH.h"
#include "Core/Messaging/Query/bcQueryProviderHandle.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Utility/bcServiceManager.h"

namespace black_cat
{
	namespace core
	{
		bc_query_provider_handle::bc_query_provider_handle(bc_message_hash p_context_hash)
			: m_context_hash(p_context_hash)
		{
		}

		bc_query_provider_handle::bc_query_provider_handle(bc_query_provider_handle&&) noexcept = default;

		bc_query_provider_handle::~bc_query_provider_handle() = default;

		bc_query_provider_handle& bc_query_provider_handle::operator=(bc_query_provider_handle&&) noexcept = default;

		void bc_query_provider_handle::reassign(delegate_t&& p_delegate)
		{
			bc_get_service<bc_query_manager>()->replace_query_provider(*this, std::move(p_delegate));
		}

		void bc_query_provider_handle::reset()
		{
			if(m_context_hash != 0)
			{
				bc_get_service<bc_query_manager>()->unregister_query_provider(*this);
				m_context_hash = 0;
			}
		}

		void bc_query_provider_handle::reset(handle_t && p_other)
		{
			reset();

			m_context_hash = p_other.m_context_hash;
			p_other.m_context_hash = 0;
		}
	}
}