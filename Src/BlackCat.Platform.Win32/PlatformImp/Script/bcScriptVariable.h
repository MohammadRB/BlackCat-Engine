// [11/10/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptRef.h"
#include "PlatformImp/Script/bcScriptReference.h"
#include "PlatformImp/Script/bcScriptContext.h"

#include "Platform/Script/bcScriptVariable.h"
#include "PlatformImp/PlatformImpPCH.h"

namespace black_cat::platform
{
	template<>
	struct bc_platform_script_variable_pack<platform::g_api_win32>
	{
		JsValueRef m_js_value;
	};

	template<platform::bc_platform TPlatform>
	bc_script_variable bc_platform_script_variable<TPlatform>::_pack_arg(void)
	{
		bc_script_variable l_result;

		return l_result;
	}

	template<platform::bc_platform TPlatform>
	bc_script_variable bc_platform_script_variable<TPlatform>::_pack_arg(bc_script_bool p_value)
	{
		bc_script_variable l_result;

		l_result.as_boolean(p_value);

		return l_result;
	}

	template<platform::bc_platform TPlatform>
	bc_script_variable bc_platform_script_variable<TPlatform>::_pack_arg(bc_script_int p_value)
	{
		bc_script_variable l_result;

		l_result.as_integer(p_value);

		return l_result;
	}

	template<platform::bc_platform TPlatform>
	bc_script_variable bc_platform_script_variable<TPlatform>::_pack_arg(bc_script_double p_value)
	{
		bc_script_variable l_result;

		l_result.as_double(p_value);

		return l_result;
	}

	template<platform::bc_platform TPlatform>
	bc_script_variable bc_platform_script_variable<TPlatform>::_pack_arg(const bc_script_variable& p_value)
	{
		return p_value;
	}

	template<platform::bc_platform TPlatform>
	bc_script_variable bc_platform_script_variable<TPlatform>::_pack_arg(const bc_script_string& p_value)
	{
		bc_script_variable l_result;

		l_result.as_string(p_value);

		return l_result;
	}

	template<platform::bc_platform TPlatform>
	bc_script_variable bc_platform_script_variable<TPlatform>::_pack_arg(const bc_script_object& p_value)
	{
		bc_script_variable l_result;

		l_result.as_object(p_value);

		return l_result;
	}

	template<platform::bc_platform TPlatform>
	bc_script_variable bc_platform_script_variable<TPlatform>::_pack_arg(const bc_script_array& p_value)
	{
		bc_script_variable l_result;

		l_result.as_array(p_value);

		return l_result;
	}

	template<platform::bc_platform TPlatform>
	void bc_platform_script_variable<TPlatform>::_unpack_arg(const bc_script_variable& p_pack, bc_script_bool* p_value)
	{
		*p_value = p_pack.as_boolean();
	}

	template<platform::bc_platform TPlatform>
	void bc_platform_script_variable<TPlatform>::_unpack_arg(const bc_script_variable& p_pack, bc_script_int* p_value)
	{
		*p_value = p_pack.as_integer();
	}

	template<platform::bc_platform TPlatform>
	void bc_platform_script_variable<TPlatform>::_unpack_arg(const bc_script_variable& p_pack, bc_script_double* p_value)
	{
		*p_value = p_pack.as_double();
	}

	template<platform::bc_platform TPlatform>
	void bc_platform_script_variable<TPlatform>::_unpack_arg(const bc_script_variable& p_pack, bc_script_variable* p_value)
	{
		*p_value = p_pack;
	}

	template<platform::bc_platform TPlatform>
	void bc_platform_script_variable<TPlatform>::_unpack_arg(const bc_script_variable& p_pack, bc_script_string* p_value)
	{
		*p_value = p_pack.as_string();
	}

	template<platform::bc_platform TPlatform>
	void bc_platform_script_variable<TPlatform>::_unpack_arg(const bc_script_variable& p_pack, bc_script_object* p_value)
	{
		*p_value = p_pack.as_object();
	}

	template<platform::bc_platform TPlatform>
	void bc_platform_script_variable<TPlatform>::_unpack_arg(const bc_script_variable& p_pack, bc_script_function* p_value)
	{
		*p_value = p_pack.as_function();
	}

	template<platform::bc_platform TPlatform>
	void bc_platform_script_variable<TPlatform>::_unpack_arg(const bc_script_variable& p_pack, bc_script_array* p_value)
	{
		*p_value = p_pack.as_array();
	}
}
