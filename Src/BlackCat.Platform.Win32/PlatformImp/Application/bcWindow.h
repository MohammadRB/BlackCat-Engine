 // [10/23/2016 MRB]

#pragma once

#include "Platform/Application/bcWindow.h"
#include "PlatformImp/PlatformImpPCH.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_window_pack< core_platform::g_api_win32 >
		{
		};
	}
}