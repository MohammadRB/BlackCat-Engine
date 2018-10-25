 // [10/10/2016 MRB]

#pragma once

#include "Platform/Script/bcScriptRuntime.h"
#include "Platform/Script/bcScriptExternalObject.h" // Make forward declared bc_script_external_object visible to clients
#include "PlatformImp/PlatformImpPCH.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_script_runtime_pack< core_platform::g_api_win32 >
		{
		public:
			JsRuntimeHandle m_js_runtime;
			bc_script_context* m_active_context;
		};
	}
}