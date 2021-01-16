// [2/11/2015 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Messaging/bcMessage.h"
#include "Core/bcExport.h"
#include "Core/bcConstant.h"

namespace black_cat
{
	namespace core
	{
		class bc_event_manager;
		class bci_event;

		using bc_event_hash = bc_message_hash;
		
		class BC_CORE_DLL bci_event : public bci_message
		{
		public:
			virtual ~bci_event() = 0;

		protected:
			bci_event(const bcCHAR* p_name);

			bci_event(const bci_event&) = default;

			bci_event& operator=(const bci_event&) = default;
		};

		class BC_CORE_DLL bc_event : public bci_event
		{
		public:
			virtual ~bc_event() = 0;

		protected:
			explicit bc_event(const bcCHAR* p_name) noexcept;

			bc_event(const bc_event&) = default;

			bc_event& operator =(const bc_event&) = default;
		};

		/**
		 * \brief Base class for application level events
		 */
		class BC_CORE_DLL bc_app_event : public bc_event
		{
		public:
			virtual ~bc_app_event() = 0;

		protected:
			explicit bc_app_event(const bcCHAR* p_name) noexcept;

			bc_app_event(const bc_app_event&) = default;

			bc_app_event& operator =(const bc_app_event&) = default;
		};

		/**
		 * \brief Base class for application level events which will be executed in render thread context
		 */
		class BC_CORE_DLL bc_app_render_event : public bc_event
		{
		public:
			virtual ~bc_app_render_event();

		protected:
			explicit bc_app_render_event(const bcCHAR* p_name) noexcept;

			bc_app_render_event(const bc_app_render_event&) = default;

			bc_app_render_event& operator =(const bc_app_render_event&) = default;
		};
	}
}