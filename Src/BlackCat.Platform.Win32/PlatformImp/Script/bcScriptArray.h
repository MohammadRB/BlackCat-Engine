// [10/12/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptRef.h"
#include "PlatformImp/Script/bcScriptContext.h"
#include "PlatformImp/Script/bcScriptVariable.h"
#include "PlatformImp/Script/bcScriptObject.h"

#include "Platform/Script/bcScriptArray.h"
#include "PlatformImp/Script/bcScriptVariable.h"
#include "PlatformImp/Script/bcScriptError.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_script_array_base_element_pack< core_platform::g_api_win32 >
		{
			bc_platform_script_array_base_element_pack(bc_script_array_base& p_array, bc_script_variable p_index)
				: m_array(p_array),
				m_index(p_index)
			{
			}

			bc_script_array_base& m_array;
			bc_script_variable m_index;
		};

		template<>
		struct bc_platform_script_array_base_pack< core_platform::g_api_win32 >
		{
		};
	}
}