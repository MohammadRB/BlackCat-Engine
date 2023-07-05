 // [10/10/2016 MRB]

#pragma once

#include "Platform/Script/bcScriptRuntime.h"
#include "PlatformImp/PlatformImpPCH.h"

namespace black_cat::platform
{
	template<>
	struct bc_platform_script_runtime_pack<platform::g_api_win32>
	{
	public:
		JsRuntimeHandle m_js_runtime;
		bc_script_context* m_active_context;
	};
}
