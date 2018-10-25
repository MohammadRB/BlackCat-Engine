// [2/11/2015 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/bcExport.h"
#include "Core/bcConstant.h"

namespace black_cat
{
	namespace core
	{
		class bc_event_manager;
		using bc_event_hash = bcUINT;

		template< class TEvent >
		struct bc_event_traits
		{
			static constexpr const bcCHAR* event_name()
			{
				return TEvent::event_name();
			}

			static constexpr bcUINT32 event_hash()
			{
				return TEvent::event_hash();
			}
		};

		class BC_CORE_DLL bc_ievent
		{
		public:
			virtual ~bc_ievent() = 0;

			virtual const bcCHAR* get_event_name() const noexcept = 0;

			virtual bc_event_hash get_event_hash() const noexcept = 0;

			static bc_event_hash get_hash(const bcCHAR* p_name) noexcept;

			template< class TEvent >
			static bool event_is(const bc_ievent& p_event)
			{
				return get_hash(bc_event_traits<TEvent>::event_name()) == p_event.get_event_hash();
			}
		};

		class BC_CORE_DLL bc_event : public bc_ievent
		{
		public:
			explicit bc_event(const bcCHAR* p_name) noexcept;

			bc_event(const bc_event&) = default;

			virtual ~bc_event() = 0;

			bc_event& operator =(const bc_event&) = default;

			const bcCHAR* get_event_name() const noexcept override;

			bc_event_hash get_event_hash() const noexcept override;

		protected:

		private:
			const bcCHAR* m_name;
			mutable bc_event_hash m_hash;
		};

		// Base class for application level events
		class BC_CORE_DLL bc_app_event : public bc_event
		{
		public:
			explicit bc_app_event(const bcCHAR* p_name) noexcept;

			bc_app_event(const bc_app_event&) = default;

			virtual ~bc_app_event() = 0;

			bc_app_event& operator =(const bc_app_event&) = default;
		};

		class BC_CORE_DLL bc_event_listener_handle
		{
		public:
			friend class bc_event_manager;

		public:
			bc_event_listener_handle();

			bc_event_listener_handle(const bcCHAR* p_event_name, bcSIZE p_event_index);

			bc_event_listener_handle(bc_event_listener_handle&& p_other) noexcept;

			~bc_event_listener_handle();

			bc_event_listener_handle& operator=(bc_event_listener_handle&& p_other) noexcept;

			void reset();

			void reset(bc_event_listener_handle&& p_other);

		protected:

		private:
			const bcCHAR* m_event_name;
			bcSIZE m_event_index;
		};
	}
}