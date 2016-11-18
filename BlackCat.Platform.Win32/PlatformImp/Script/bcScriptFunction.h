// [10/12/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptRef.h"
#include "PlatformImp/Script/bcScriptReference.h"
#include "PlatformImp/Script/bcScriptContext.h"
#include "PlatformImp/Script/bcScriptVariable.h"

#include "Platform/Script/bcScriptFunction.h"
#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/Script/bcScriptError.h"

namespace black_cat
{
	namespace platform
	{
		template< >
		struct bc_platform_script_function_base_pack< core_platform::g_api_win32 >
		{
			JsValueRef m_js_function;
		};
	}
}