// [10/08/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"

namespace black_cat
{
	namespace platform
	{
		enum class bc_lock_operation
		{
			light = 0,
			medium = 1,
			heavy = 2
		};

		enum class bc_mutex_type
		{
			plain = 0x01,
			Try = 0x02,
			timed = 0x04,
			recursive = 0x100
		};

		enum class bc_lock_property
		{
			adapt,
			defer,
			Try
		};

		enum class bc_cv_status
		{
			timeout,
			no_timeout
		};

		enum class bc_future_status
		{
			ready,
			timeout,
			deferred
		};

		enum class bc_memory_order
		{
			relaxed,
			consume,
			acquire,
			release,
			acqrel,
			seqcst
		};
	}
}