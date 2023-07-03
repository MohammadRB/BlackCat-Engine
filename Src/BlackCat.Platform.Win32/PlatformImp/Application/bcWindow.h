 // [23/10/2016 MRB]

#pragma once

#include "Platform/Application/bcWindow.h"
#include "PlatformImp/PlatformImpPCH.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_window_pack<g_api_win32>
		{
		};
	}
}