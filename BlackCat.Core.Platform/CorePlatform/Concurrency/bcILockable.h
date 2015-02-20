// [10/08/2014 MRB]

#pragma once

#include "coreplatform/coreplatformpch.h"
#include "coreplatform/bctype.h"
#include "CorePlatform/Concurrency/bcConcurrencyDef.h"
#include "CorePlatform/bcCorePlatformUtility.h"

namespace black_cat
{
	namespace core_platform
	{
		class bc_Ilockable : private bc_no_copy
		{
		public:
			bc_Ilockable() {}

			virtual ~bc_Ilockable() {}

			virtual void lock() = 0;

			virtual void unlock() = 0;

			virtual bool try_lock() = 0;

		protected:

		private:
		};

		class bc_Itimed_lockable : public bc_Ilockable
		{
		public:
			bc_Itimed_lockable() {}

			virtual ~bc_Itimed_lockable() {}

			virtual bool try_lock_for(const bcUINT64 p_nano) = 0;

		protected:

		private:
		};

		class bc_Ihybrid_lockable : public bc_Ilockable
		{
		public:
			bc_Ihybrid_lockable() {}

			virtual ~bc_Ihybrid_lockable() {}

			virtual void lock(core_platform::bc_lock_operation p_lock_operation) = 0;

			virtual bool try_lock(core_platform::bc_lock_operation p_lock_operation) = 0;

		protected:

		private:
		};
	}
}