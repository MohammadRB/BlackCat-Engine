// [2/11/2015 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/bcExport.h"
#include "Core/bcConstant.h"

namespace black_cat
{
	namespace core
	{
		using bc_event_hash = bcUINT;

		template< class TEvent >
		struct bc_event_traits
		{
			static const bcCHAR* event_name()
			{
				return TEvent::event_name();
			}
		};

		class BC_CORE_DLL bc_ievent
		{
		public:
			virtual ~bc_ievent() = 0;

			virtual bc_event_hash get_event_hash() const noexcept(true) = 0;

			virtual const bcCHAR* get_event_name() const noexcept(true) = 0;

			static bc_event_hash get_hash(const bcCHAR* p_name) noexcept(true);

			template< class TEvent >
			static bool event_is(const bc_ievent& p_event)
			{
				return get_hash(bc_event_traits<TEvent>::event_name()) == p_event.get_event_hash();
			}
		};

		class BC_CORE_DLL bc_event : public bc_ievent
		{
		public:
			explicit bc_event(const bcCHAR* p_name) noexcept(true);

			bc_event(const bc_event&) = default;

			virtual ~bc_event() = 0;

			bc_event& operator =(const bc_event&) = default;

			bc_event_hash get_event_hash() const noexcept(true) override;

			const bcCHAR* get_event_name() const noexcept(true) override;

		protected:

		private:
			const bcCHAR* m_name;
			mutable bc_event_hash m_hash;
		};

		// Base class for application level events
		class BC_CORE_DLL bc_app_event : public bc_event
		{
		public:
			explicit bc_app_event(const bcCHAR* p_name) noexcept(true);

			bc_app_event(const bc_app_event&) = default;

			virtual ~bc_app_event() = 0;

			bc_app_event& operator =(const bc_app_event&) = default;
		};
	}
}