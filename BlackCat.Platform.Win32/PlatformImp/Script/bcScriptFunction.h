// [10/12/2016 MRB]

#pragma once

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
		template< typename TR, typename ...TA >
		struct bc_platform_script_function_pack< core_platform::g_api_win32, TR, TA... >
		{
			JsValueRef m_js_function;
		};

		template< typename TR, typename ...TA >
		JsValueRef CALLBACK _js_call(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			// TODO Make a chiose for more and less argument count than expected
			bcAssert(p_argument_count == sizeof...(TA) + 1);

			auto l_function = reinterpret_cast< typename bc_script_function< TR, TA... >::callback_t >(p_callback_state);
			bc_script_var_pack< TA... > l_pack;

			std::transform
			(
				std::next(p_arguments), // First argument is 'this' object
				std::next(p_arguments, p_argument_count),
				std::begin(l_pack),
				[](JsValueRef p_js_value)
				{
					bc_script_variable l_variable;
					l_variable.get_platform_pack().m_js_value = p_js_value;

					return l_variable;
				}
			);

			bc_script_variable l_callback_result = bc_script_function< TR, TA... >::_call_callback
			(
				std::ptr_fun(l_function),
				l_pack.data(),
				l_pack.size()
			);

			return l_callback_result.get_platform_pack().m_js_value;
		}

		template< core_platform::bc_platform TPlatform, typename TR, typename ... TA >
		bc_platform_script_function< TPlatform, TR(TA...) >::bc_platform_script_function()
			: bc_platform_script_reference()
		{
			m_pack.m_js_function = JS_INVALID_REFERENCE;
		}

		template< core_platform::bc_platform TPlatform, typename TR, typename ... TA >
		bc_platform_script_function< TPlatform, TR(TA...) >::bc_platform_script_function(bc_script_context& p_context, callback_t p_callback)
		{
			static_assert(sizeof(callback_t) == sizeof(void*), "Function pointer size isn't equal to size of void*");

			bc_chakra_call l_call(p_context);

			l_call = JsCreateFunction(&_js_call< TR, TA... >, p_callback, &m_pack.m_js_function);
		}

		template< core_platform::bc_platform TPlatform, typename TR, typename ... TA >
		bc_platform_script_function< TPlatform, TR(TA...) >::bc_platform_script_function(const bc_platform_script_function& p_other) noexcept
		{
			operator=(p_other);
		}

		template< core_platform::bc_platform TPlatform, typename TR, typename ... TA >
		bc_platform_script_function< TPlatform, TR(TA...) >::~bc_platform_script_function()
		{	
		}

		template< core_platform::bc_platform TPlatform, typename TR, typename ... TA >
		bc_platform_script_function< TPlatform, TR(TA...) >& bc_platform_script_function< TPlatform, TR(TA...) >::operator=(const bc_platform_script_function& p_other) noexcept
		{
			bc_platform_script_reference<TPlatform>::operator=(p_other);
			m_pack.m_js_function = p_other.m_pack.m_js_function;

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename TR, typename ... TA >
		TR bc_platform_script_function< TPlatform, TR(TA...) >::operator()(bc_script_var_pack< bc_script_variable, TA... >& p_args) const
		{
			bc_chakra_call l_call;
			bc_script_variable l_call_result;
			TR l_result;
			core::bc_array< JsValueRef, sizeof...(TA) + 1 > l_args;

			std::transform
			(
				std::begin(p_args),
				std::end(p_args),
				std::begin(l_args),
				[](bc_script_variable& p_js_value)
				{
					return p_js_value.get_platform_pack().m_js_value;
				}
			);

			l_call = JsCallFunction
			(
				m_pack.m_js_function,
				l_args.data(),
				l_args.size(),
				&l_call_result.get_platform_pack().m_js_value
			);

			bc_script_variable::_unpack_arg(l_call_result, &l_result);

			return l_result;
		}

		template< core_platform::bc_platform TPlatform, typename TR, typename ...TA >
		TR bc_platform_script_function< TPlatform, TR(TA...) >::operator()(bc_script_variable& p_this, TA&... p_args) const
		{
			bc_script_variable l_call_result;
			bc_script_var_pack< bc_script_variable, TA... > l_packed_args;

			bc_platform_script_function< TPlatform, TR(bc_script_variable&, TA...) >::_pack_args(l_packed_args, p_this, p_args...);

			return (*this)(l_packed_args);
		}

		template< core_platform::bc_platform TPlatform, typename TR, typename ...TA >
		bool bc_platform_script_function< TPlatform, TR(TA...) >::is_valid() const noexcept
		{
			return m_pack.m_js_function != JS_INVALID_REFERENCE;
		}
	}
}