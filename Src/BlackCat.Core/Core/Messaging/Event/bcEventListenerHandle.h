// [2/11/2015 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/bcExport.h"
#include "Core/Messaging/Event/bcEvent.h"
#include "Core/Messaging/Event/bcEventHandler.h"

namespace black_cat
{
	namespace core
	{
		class bc_event_manager;

		class BC_CORE_DLL bc_event_listener_handle : platform::bc_no_copy_move
		{
		public:
			friend class bc_event_manager;
			using event_handler_type = bc_event_handler<void(bci_event&)>;
			using delegate_type = event_handler_type::delegate_type;

		public:
			bc_event_listener_handle();

			bc_event_listener_handle(bc_event_listener_handle&& p_other) noexcept;

			~bc_event_listener_handle();

			bc_event_listener_handle& operator=(bc_event_listener_handle&& p_other) noexcept;

			/**
			 * \brief Must be called after move construction or assignment, so we do not end up with dangled pointers in delegates
			 * \param p_delegate 
			 */
			void reassign(delegate_type&& p_delegate);

			void reset();

			void reset(bc_event_listener_handle&& p_other);

		private:
			bc_event_listener_handle(const bcCHAR* p_event_name, bcSIZE p_event_index);

			const bcCHAR* m_event_name;
			bcSIZE m_event_index;
		};
	}
}