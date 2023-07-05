// [21/03/2020 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatform/Utility/bcNoCopy.h"

namespace black_cat::core
{
	template< class THandle, class TDelegate >
	class bc_message_handle : platform::bc_no_copy
	{
	public:
		using handle_t = THandle;
		using delegate_t = TDelegate;

	public:
		/**
			 * \brief Must be called after move construction or assignment, so we do not end up with dangled pointers in delegates
			 * \param p_delegate
			 */
		virtual void reassign(delegate_t&& p_delegate) = 0;

		virtual void reset() = 0;

		virtual void reset(handle_t&& p_other) = 0;

	protected:
		bc_message_handle();

		bc_message_handle(bc_message_handle&& p_other) noexcept;

		~bc_message_handle();

		bc_message_handle& operator=(bc_message_handle&& p_other) noexcept;
	};

	template< class THandle, class TDelegate >
	bc_message_handle< THandle, TDelegate >::bc_message_handle() = default;

	template< class THandle, class TDelegate >
	bc_message_handle< THandle, TDelegate >::bc_message_handle(bc_message_handle&& p_other) noexcept = default;

	template< class THandle, class TDelegate >
	bc_message_handle< THandle, TDelegate >::~bc_message_handle() = default;

	template< class THandle, class TDelegate >
	bc_message_handle< THandle, TDelegate >& bc_message_handle< THandle, TDelegate >::operator=(bc_message_handle&& p_other) noexcept = default;
}
