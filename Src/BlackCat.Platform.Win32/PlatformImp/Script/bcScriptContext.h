// [10/11/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptRef.h"
#include "PlatformImp/Script/bcScriptReference.h"
#include "PlatformImp/Script/bcScriptRuntime.h"

#include "Platform/Script/bcScriptContext.h"
#include "PlatformImp/PlatformImpPCH.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_script_context_pack< core_platform::g_api_win32 >
		{
			JsContextRef m_js_context;
		};
	}
}
