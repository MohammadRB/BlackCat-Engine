// [21/03/2020 MRB]

#pragma once

#include "Core/bcExport.h"
#include "Core/Messaging/bcMessageHandle.h"
#include "Core/Messaging/Query/bcQueryContext.h"
#include "Core/Utility/bcDelegate.h"

namespace black_cat::core
{
	class bc_query_manager;
		
	class BC_CORE_DLL bc_query_provider_handle final : public bc_message_handle<bc_query_provider_handle, bc_delegate< bc_query_context_ptr() >>
	{
	public:
		friend class bc_query_manager;

	public:
		bc_query_provider_handle();
			
		bc_query_provider_handle(bc_query_provider_handle&&) noexcept;

		~bc_query_provider_handle();

		bc_query_provider_handle& operator=(bc_query_provider_handle&&) noexcept;

		/**
			 * \brief Must be called after move construction or assignment, so we do not end up with dangled pointers in delegates
			 * \param p_delegate
			 */
		void reassign(delegate_t&& p_delegate) override;
			
		void reset() override;
			
		void reset(handle_t&& p_other) override;

	private:
		explicit bc_query_provider_handle(bc_query_context_hash p_context_hash);

		bc_query_context_hash m_context_hash;
	};
}
