 // [10/12/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptContext.h"
#include "PlatformImp/Script/bcScriptVariable.h"
#include "PlatformImp/Script/bcScriptPrototype.h"

#include "Platform/Script/bcScriptObject.h"
#include "PlatformImp/PlatformImpPCH.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_script_object_pack< core_platform::g_api_win32 >
		{
			JsValueRef m_js_object;
		};
	}
}