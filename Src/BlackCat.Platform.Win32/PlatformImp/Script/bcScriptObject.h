 // [10/12/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptRef.h"
#include "PlatformImp/Script/bcScriptContext.h"
#include "PlatformImp/Script/bcScriptVariable.h"
#include "PlatformImp/Script/bcScriptPrototype.h"

#include "Platform/Script/bcScriptObject.h"
#include "Platform/Script/bcScriptRuntime.hpp"
#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/Script/bcScriptError.h"

namespace black_cat
{
	namespace platform
	{
		template< typename T >
		void CHAKRA_CALLBACK _js_object_finalizer(void* p_object)
		{
			bc_script_runtime::destroy_native(static_cast<bc_script_external_object<T>*>(p_object));
		}

		template<>
		struct bc_platform_script_object_pack<core_platform::g_api_win32>
		{
			JsValueRef m_js_object;
		};

		template<>
		template<typename T>
		bc_platform_script_object< core_platform::g_api_win32 >::bc_platform_script_object(bc_script_context& p_context, bc_script_prototype< T >& p_prototype, T&& p_native_object) noexcept
		{
			bc_chakra_call l_call(p_context);

			if (!p_prototype.is_valid())
			{
				throw bc_invalid_operation_exception("Invalid prototype object");
			}

			bc_script_external_object<T>* l_native_object = bc_script_runtime::create_native<T, T&&>(std::move(p_native_object));

			l_call = JsCreateExternalObject(l_native_object, &_js_object_finalizer<T>, &m_pack.m_js_object);
			l_call = JsSetPrototype(m_pack.m_js_object, p_prototype.get_platform_pack().m_js_prototype);
		}
	}
}