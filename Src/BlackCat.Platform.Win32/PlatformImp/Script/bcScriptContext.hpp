// [10/11/2016 MRB]

#pragma once

/*
* This file is created duo to circular dependency between other implementation headers and bcScriptContext.h
*/

#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/Script/bcScriptContext.h"
#include "PlatformImp/Script/bcScriptExternalObject.h"
#include "PlatformImp/Script/bcScriptRuntime.h"
#include "PlatformImp/Script/bcScriptReference.h"
#include "PlatformImp/Script/bcScriptVariable.h"
#include "PlatformImp/Script/bcScriptObject.h"
#include "PlatformImp/Script/bcScriptString.h"
#include "PlatformImp/Script/bcScriptError.h"
#include "PlatformImp/Script/bcScriptFunction.h"
#include "PlatformImp/Script/bcScriptArray.h"
#include "PlatformImp/Script/bcScriptGlobalPrototypeBuilder.h"
#include "PlatformImp/Script/bcScriptPrototypeBuilder.h"
#include "PlatformImp/Script/bcScriptPrototype.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		inline bc_platform_script_context< core_platform::g_api_win32 >::bc_platform_script_context()
		{
			m_runtime = nullptr;
			m_pack.m_js_context = JS_INVALID_REFERENCE;
		}

		template<>
		inline bc_platform_script_context< core_platform::g_api_win32 >::bc_platform_script_context(bc_script_runtime& p_runtime) noexcept
			: m_runtime(&p_runtime)
		{
		}
		
		template<>
		inline bc_platform_script_context< core_platform::g_api_win32 >::bc_platform_script_context(const bc_platform_script_context& p_other) noexcept
			: bc_platform_script_reference(p_other)
		{
			m_runtime = p_other.m_runtime;
			m_pack.m_js_context = p_other.m_pack.m_js_context;
		}

		template<>
		inline bc_platform_script_context< core_platform::g_api_win32 >::~bc_platform_script_context()
		{
		}

		template<>
		inline bc_platform_script_context< core_platform::g_api_win32 >& bc_platform_script_context< core_platform::g_api_win32 >::operator=(const bc_platform_script_context& p_other) noexcept
		{
			bc_platform_script_reference::operator=(p_other);
			m_runtime = p_other.m_runtime;
			m_pack.m_js_context = p_other.m_pack.m_js_context;

			return *this;
		}

		template<>
		inline bc_script_global_prototype_builder bc_platform_script_context< core_platform::g_api_win32 >::create_global_prototype_builder()
		{
			bc_script_global_prototype_builder l_prototype_builder(*this);

			return l_prototype_builder;
		}

		template<>
		inline void bc_platform_script_context< core_platform::g_api_win32 >::register_global_prototype(bc_script_global_prototype_builder&& p_global_prototype)
		{
		}

		template<>
		inline bc_script_variable bc_platform_script_context< core_platform::g_api_win32 >::create_undefined()
		{
			bc_script_variable l_var;

			JsGetUndefinedValue(&l_var.get_platform_pack().m_js_value);

			return l_var;
		}

		template<>
		inline bc_script_variable bc_platform_script_context< core_platform::g_api_win32 >::create_null()
		{
			bc_script_variable l_var;

			JsGetNullValue(&l_var.get_platform_pack().m_js_value);

			return l_var;
		}

		template<>
		inline bc_script_variable bc_platform_script_context< core_platform::g_api_win32 >::create_variable(bc_script_bool p_bool)
		{
			return bc_script_variable(*this, p_bool);
		}

		template<>
		inline bc_script_variable bc_platform_script_context< core_platform::g_api_win32 >::create_variable(bc_script_int p_integer)
		{
			return bc_script_variable(*this, p_integer);
		}

		template<>
		inline bc_script_variable bc_platform_script_context< core_platform::g_api_win32 >::create_variable(bc_script_double p_double)
		{
			return bc_script_variable(*this, p_double);
		}

		template<>
		inline bc_script_variable bc_platform_script_context< core_platform::g_api_win32 >::create_variable(bc_script_string& p_string)
		{
			return bc_script_variable(*this, p_string);
		}

		template<>
		inline bc_script_variable bc_platform_script_context< core_platform::g_api_win32 >::create_variable(bc_script_object& p_object)
		{
			return bc_script_variable(*this, p_object);
		}

		template<>
		inline bc_script_variable bc_platform_script_context< core_platform::g_api_win32 >::create_variable(bc_script_array& p_array)
		{
			return bc_script_variable(*this, p_array);
		}

		template<>
		inline bc_script_variable bc_platform_script_context< core_platform::g_api_win32 >::create_variable(bc_script_function& p_function)
		{
			return bc_script_variable(*this, p_function);
		}

		template<>
		inline bc_script_variable bc_platform_script_context< core_platform::g_api_win32 >::create_variable(bc_script_error& p_error)
		{
			return bc_script_variable(*this, p_error);
		}

		template<>
		inline bc_script_string bc_platform_script_context< core_platform::g_api_win32 >::create_string(core::bc_wstring& p_string)
		{
			return bc_script_string(*this, p_string);
		}

		template<>
		template< typename T >
		bc_script_object bc_platform_script_context< core_platform::g_api_win32 >::create_object(bc_script_prototype< T >& p_prototype, T&& p_native_object)
		{
			return bc_script_object(*this, p_prototype, std::move(p_native_object));
		}

		template<>
		inline bc_script_array bc_platform_script_context< core_platform::g_api_win32 >::create_array(bcSIZE p_length)
		{
			return bc_script_array(*this, p_length);
		}

		/*template<>
		template< typename TR, typename ...TA >
		bc_script_function< TR(TA ...) > bc_platform_script_context< core_platform::g_api_win32 >::create_function(typename bc_script_function< TR, TA... >::callback_t p_delegate)
		{
			return bc_script_function< TR(TA ...) >(*this, p_delegate);
		}*/

		template<>
		template< typename T >
		bc_script_prototype_builder< T > bc_platform_script_context< core_platform::g_api_win32 >::create_prototype_builder()
		{
			BC_ASSERT(bc_script_external_object<T>::get_meta_data() == nullptr);

			auto* l_meta_data = m_runtime->_create_external_object_meta_data< T >();

			bc_script_external_object< T >::set_meta_data_ptr(l_meta_data);

			return bc_script_prototype_builder< T >(*this);
		}

		template< core_platform::bc_platform TPlatform >
		template< typename T >
		bc_script_prototype< T > bc_platform_script_context< TPlatform >::create_prototype(bc_script_prototype_builder< T >& p_prototype_builder)
		{
			bc_script_prototype< T > l_prototype;

			l_prototype.get_platform_pack().m_js_ctor_function = p_prototype_builder.get_platform_pack().m_js_ctor_function;
			l_prototype.get_platform_pack().m_js_prototype = p_prototype_builder.get_platform_pack().m_js_prototype;

			bc_script_external_object< T >::set_prototype(l_prototype);

			return l_prototype;
		}

		template< core_platform::bc_platform TPlatform >
		template< typename T >
		void bc_platform_script_context< TPlatform >::register_prototype(bcWCHAR* p_object_name, bc_script_prototype< T >& p_object_prototype)
		{
			bc_chakra_call l_call(*this);
			JsPropertyIdRef l_prototype_name;

			if (p_object_prototype.get_platform_pack().m_js_ctor_function == JS_INVALID_REFERENCE)
			{
				throw bc_invalid_operation_exception("Prototype doesn't include constructor function signature");
			}

			JsValueRef l_global = get_global().get_platform_pack().m_js_object;

			l_call = JsGetPropertyIdFromName(p_object_name, &l_prototype_name);
			l_call.throw_if_faild();

			l_call = JsSetProperty(l_global, l_prototype_name, p_object_prototype.get_platform_pack().m_js_ctor_function, true);
			l_call.throw_if_faild();
		}

		template< core_platform::bc_platform TPlatform >
		bc_script_error bc_platform_script_context< TPlatform >::create_error(const core::bc_wstring& p_message)
		{
			return bc_script_error(*this, p_message);
		}

		template<>
		inline bool bc_platform_script_context< core_platform::g_api_win32 >::has_exception() const noexcept
		{
			bc_chakra_call l_call(const_cast< bc_script_context& >(*this));
			bool l_result;

			l_call = JsHasException(&l_result);

			return l_result;
		}

		template<>
		inline bc_script_error bc_platform_script_context< core_platform::g_api_win32 >::get_clear_exception()
		{
			bc_chakra_call l_call(*this);
			bc_script_error l_error;

			l_call = JsGetAndClearException(&l_error.get_platform_pack().m_js_error);

			return l_error;
		}

		template<>
		inline void bc_platform_script_context< core_platform::g_api_win32 >::set_exception(bc_script_error& p_error)
		{
			bc_chakra_call l_call(*this);

			l_call = JsSetException(p_error.get_platform_pack().m_js_error);
		}

		template<>
		inline bc_script_object bc_platform_script_context< core_platform::g_api_win32 >::get_global() const
		{
			bc_chakra_call l_call(const_cast< bc_platform_script_context& >(*this));
			bc_script_object l_object;

			l_call = JsGetGlobalObject(&l_object.get_platform_pack().m_js_object);

			return l_object;
		}

		template<>
		inline bool bc_platform_script_context< core_platform::g_api_win32 >::is_valid() const noexcept
		{
			return m_pack.m_js_context != JS_INVALID_REFERENCE;
		}
	}
}