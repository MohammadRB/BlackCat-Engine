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
		class bc_ievent;

		using bc_event_hash = bc_message_hash;
		
		class BC_CORE_DLL bc_ievent : public bc_imessage
		{
		public:
			virtual ~bc_ievent() = 0;

		protected:
			bc_ievent(const bcCHAR* p_name);

			bc_ievent(const bc_ievent&) = default;

			bc_ievent& operator=(const bc_ievent&) = default;
		};

		class BC_CORE_DLL bc_event : public bc_ievent
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