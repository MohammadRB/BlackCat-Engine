 // [11/10/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptRef.h"
#include "PlatformImp/Script/bcScriptReference.h"
#include "PlatformImp/Script/bcScriptRuntime.h"

#include "Platform/Script/bcScriptByteCode.h"
#include "PlatformImp/PlatformImpPCH.h"

namespace black_cat::platform
{
	template<>
	struct bc_platform_script_bytecode_pack< platform::g_api_win32 >
	{
		JsValueRef m_js_parsed_script;
	};
}
