// [2/11/2015 MRB]

#pragma once

#include "Core/CorePCH.h"

namespace black_cat
{
	namespace core
	{
		using bc_event_hash = bcUINT32;

		class BC_COREDLL_EXP bc_ievent
		{
		public:
			virtual ~bc_ievent() = 0;

			virtual bc_event_hash get_event_hash() const noexcept(true) = 0;

			virtual const bcCHAR* get_event_name() const noexcept(true) = 0;

			static bc_event_hash get_hash(const bcCHAR* p_name) noexcept(true);
		};

		class BC_COREDLL_EXP bc_event : public bc_ievent
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
		class BC_COREDLL_EXP bc_app_event : public bc_event
		{
		public:
			explicit bc_app_event(const bcCHAR* p_name) noexcept(true);

			bc_app_event(const bc_app_event&) = default;

			virtual ~bc_app_event() = 0;

			bc_app_event& operator =(const bc_app_event&) = default;
		};
	}
}