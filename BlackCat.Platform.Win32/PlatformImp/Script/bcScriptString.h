 // [10/12/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptReference.h"
#include "PlatformImp/Script/bcScriptContext.h"

#include "Platform/Script/bcScriptString.h"
#include "PlatformImp/PlatformImpPCH.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_script_string_pack< core_platform::g_api_win32 >
		{
			JsValueRef m_js_string;
		};
	}
}