// [10/11/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptRef.h"
#include "PlatformImp/Script/bcScriptReference.h"
#include "PlatformImp/Script/bcScriptContext.h"

#include "Platform/Script/bcScriptVariable.h"
#include "PlatformImp/PlatformImpPCH.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_script_variable_pack< core_platform::g_api_win32 >
		{
			JsValueRef m_js_value;
		};

		template< core_platform::bc_platform TPlatform >
		template< typename TR, typename ... TA >
		bc_platform_script_variable<TPlatform>::bc_platform_script_variable(bc_script_context& p_context, bc_script_function<TR(TA...)>& p_function)
		{
			as_function(p_function);
		}

		template< core_platform::bc_platform TPlatform >
		template< typename TR, typename ... TA >
		void bc_platform_script_variable< TPlatform >::as_function(const bc_script_function< TR(TA ...) >& p_function)
		{
			m_pack.m_js_value = const_cast< bc_script_function< TR(TA ...) >& >(p_function).get_platform_pack().m_js_function;
		}

		template< core_platform::bc_platform TPlatform >
		template< typename TR, typename ... TA >
		bc_script_function< TR(TA ...) > bc_platform_script_variable< TPlatform >::as_function() const
		{
			BC_ASSERT(is_function());

			bc_script_function< TR(TA ...) > l_function;

			l_function.get_platform_pack().m_js_function = m_pack.m_js_value;

			return l_function;
		}

		template<>
		template< typename T >
		void bc_platform_script_variable< core_platform::g_api_win32 >::as_array(const bc_script_array< T >& p_array)
		{
			as_object(p_array);
		}

		template<>
		template< typename T >
		bc_script_array< T > bc_platform_script_variable< core_platform::g_api_win32 >::as_array() const
		{
			BC_ASSERT(is_array());

			bc_script_array< T > l_array;

			static_cast< bc_script_variable& >(l_array).get_platform_pack().m_js_value = m_pack.m_js_value;

			return l_array;
		}
	}
}
