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
		class bc_ievent;

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
			static bool event_is(const bc_ievent& p_event);

			template< class TEvent >
			static TEvent* event_as(bc_ievent& p_event);

			template< class TEvent >
			static const TEvent* event_as(const bc_ievent& p_event);

		protected:
			bc_ievent() = default;

			bc_ievent(const bc_ievent&) = default;

			bc_ievent& operator=(const bc_ievent&) = default;
		};

		class BC_CORE_DLL bc_event : public bc_ievent
		{
		public:
			virtual ~bc_event() = 0;

			const bcCHAR* get_event_name() const noexcept override;

			bc_event_hash get_event_hash() const noexcept override;

		protected:
			explicit bc_event(const bcCHAR* p_name) noexcept;

			bc_event(const bc_event&) = default;

			bc_event& operator =(const bc_event&) = default;

		private:
			const bcCHAR* m_name;
			mutable bc_event_hash m_hash;
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

		template< class TEvent >
		bool bc_ievent::event_is(const bc_ievent& p_event)
		{
			constexpr auto l_hash = bc_event_traits<TEvent>::event_hash();
			return l_hash == p_event.get_event_hash();
		}

		template< class TEvent >
		TEvent* bc_ievent::event_as(bc_ievent& p_event)
		{
			if (event_is<TEvent>(p_event))
			{
				return static_cast<TEvent*>(&p_event);
			}

			return nullptr;
		}

		template< class TEvent >
		const TEvent* bc_ievent::event_as(const bc_ievent& p_event)
		{
			return event_as<TEvent>(const_cast<bc_ievent&>(p_event));
		}
	}
}