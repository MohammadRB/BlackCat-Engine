// [10/08/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"

namespace black_cat
{
	namespace core_platform
	{
		enum class bc_platform
		{
			unknown,
			win32
		};

		constexpr bc_platform g_current_platform = bc_platform::win32;
	}
}