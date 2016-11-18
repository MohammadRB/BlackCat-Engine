 // [10/12/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptRef.h"
#include "PlatformImp/Script/bcScriptContext.h"
#include "PlatformImp/Script/bcScriptVariable.h"
#include "PlatformImp/Script/bcScriptPrototype.h"

#include "Platform/Script/bcScriptObject.h"
#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/Script/bcScriptError.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_script_object_pack< core_platform::g_api_win32 >
		{
			JsValueRef m_js_object;
		};

		template<>
		template< typename T >
		bc_platform_script_object< core_platform::g_api_win32 >::bc_platform_script_object(bc_script_context& p_context, bc_script_prototype< T >& p_prototype, T&& p_native_object) noexcept
		{
			bc_chakra_call l_call(p_context);
			bc_script_external_object< T >* l_native_object;

			if (!p_prototype.is_valid())
			{
				throw bc_invalid_operation_exception("Invalid prototype object");
			}

			l_native_object = bc_script_runtime::create_native< T, T&& >(std::move(p_native_object));

			l_call = JsCreateExternalObject(l_native_object, reinterpret_cast< JsFinalizeCallback >(&_js_object_finalizer< T >), &m_pack.m_js_object);
			l_call = JsSetPrototype(m_pack.m_js_object, p_prototype.get_platform_pack().m_js_prototype);
		}
	}
}