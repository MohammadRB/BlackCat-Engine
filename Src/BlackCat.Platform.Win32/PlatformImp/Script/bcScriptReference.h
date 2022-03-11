 // [10/11/2016 MRB]

#pragma once

#include "Platform/Script/bcScriptReference.h"
#include "PlatformImp/PlatformImpPCH.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_script_reference_pack<platform::g_api_win32>
		{
		};
	}
}