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
		};

		class BC_COREDLL_EXP bc_event : public bc_ievent
		{
		public:
			bc_event(const bcCHAR* p_name) noexcept(true);

			bc_event(const bc_event&) = default;

			virtual ~bc_event() = 0;

			bc_event& operator =(const bc_event&) = default;

			bc_event_hash get_event_hash() const noexcept(true) override;

			const bcCHAR* get_event_name() const noexcept(true) override;

			static bc_event_hash get_hash(const bcCHAR* p_name) noexcept(true);

		protected:

		private:
			const bcCHAR* m_name;
			mutable bc_event_hash m_hash;

		};
	}
}