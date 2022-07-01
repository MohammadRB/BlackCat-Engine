// [10/13/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptContext.h"
#include "PlatformImp/Script/bcScriptObject.h"

#include "Core/Utility/bcLogger.h"
#include "Platform/bcException.h"
#include "Platform/Script/bcScriptPrototypeBuilder.h"
#include "Platform/Script/bcScriptRuntime.hpp"
#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/Script/bcScriptRuntime.h"
#include "PlatformImp/Script/bcScriptObject.h"
#include "PlatformImp/Script/bcScriptFunction.h"
#include "PlatformImp/Script/bcScriptPrototype.h"
#include "PlatformImp/Script/bcScriptError.h"

namespace black_cat
{
	namespace platform
	{
		inline void _bc_log_wrong_number_of_arguments()
		{
			core::bc_log(core::bc_log_type::warning, bcL("provided argument counts in script function call is not equal to number of arguments which native function accepts"));
		}
		
		template<typename T>
		void CHAKRA_CALLBACK _js_object_ctor_finalizer(void* p_object)
		{
			bc_script_runtime::destroy_native(static_cast<bc_script_external_object<T>*>(p_object));
		}

		template<typename T, bcSIZE TArgCount>
		JsValueRef CHAKRA_CALLBACK _js_object_ctor(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			BC_ASSERT(p_is_construct_call);

			if(p_argument_count != TArgCount + 1) // first arg is this
			{
				_bc_log_wrong_number_of_arguments();
			}

			p_argument_count = std::min(static_cast<bcUINT16>(TArgCount + 1), p_argument_count);
			
			bc_script_arg_pack<TArgCount> l_args_pack;
			JsValueRef l_script_object;

			std::transform
			(
				std::next(p_arguments), // First argument is 'this' object
				std::next(p_arguments, p_argument_count),
				std::begin(l_args_pack),
				[](JsValueRef p_js_value)
				{
					bc_script_variable l_variable;
					l_variable.get_platform_pack().m_js_value = p_js_value;

					return l_variable;
				}
			);

			bc_script_external_object<T>* l_native_object = bc_script_function::call_callback(&bc_script_runtime::create_native<T>,l_args_pack);

			bc_chakra_call l_call = JsCreateExternalObject(l_native_object, &_js_object_ctor_finalizer<T>, &l_script_object);
			l_call = JsSetPrototype(l_script_object, l_native_object->get_prototype().get_platform_pack().m_js_prototype);

			return l_script_object;
		}

		template<typename TM>
		JsValueRef CHAKRA_CALLBACK _js_getter_default(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			if (p_argument_count != 1)
			{
				_bc_log_wrong_number_of_arguments();
			}

			JsValueRef l_this = p_arguments[0];
			TM* l_external_variable = reinterpret_cast<TM*>(p_callback_state);

			bc_script_variable l_variable = bc_script_variable::_pack_arg(*l_external_variable);

			return l_variable.get_platform_pack().m_js_value;
		}

		template<typename T, typename TM>
		JsValueRef CHAKRA_CALLBACK _js_object_getter_default(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			if (p_argument_count != 1)
			{
				_bc_log_wrong_number_of_arguments();
			}

			const JsValueRef l_this = p_arguments[0];
			bc_script_external_object<T>* l_external_object;

			bc_chakra_call(JsGetExternalData(l_this, reinterpret_cast<void**>(&l_external_object))).throw_if_faild();
			bcUINT32 l_member_variable_index = reinterpret_cast<bcUINT32>(p_callback_state);

			TM l_member_variable = l_external_object->template get_member_variable<TM>(l_member_variable_index);
			bc_script_variable l_variable = bc_script_variable::_pack_arg(l_member_variable);

			return l_variable.get_platform_pack().m_js_value;
		}

		template<typename TM>
		JsValueRef CHAKRA_CALLBACK _js_setter_default(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			if (p_argument_count != 2)
			{
				_bc_log_wrong_number_of_arguments();
			}

			bc_script_variable l_arg;

			JsValueRef l_this = p_arguments[0];
			l_arg.get_platform_pack().m_js_value = p_arguments[1];
			TM* l_external_variable = reinterpret_cast<TM*>(p_callback_state);

			TM l_variable;
			bc_script_variable::_unpack_arg(l_arg, &l_variable);

			*l_external_variable = l_variable;

			return JS_INVALID_REFERENCE;
		}

		template<typename T, typename TM>
		JsValueRef CHAKRA_CALLBACK _js_object_setter_default(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			if (p_argument_count != 2)
			{
				_bc_log_wrong_number_of_arguments();
			}

			JsValueRef l_this;
			bc_script_variable l_arg;
			bc_script_external_object<T>* l_external_object;

			l_this = p_arguments[0];
			l_arg.get_platform_pack().m_js_value = p_arguments[1];
			bc_chakra_call(JsGetExternalData(l_this, reinterpret_cast<void**>(&l_external_object))).throw_if_faild();
			bcUINT32 l_member_variable_index = reinterpret_cast<bcUINT32>(p_callback_state);

			TM l_variable;
			bc_script_variable::_unpack_arg(l_arg, &l_variable);
			l_external_object->template set_member_variable<TM>(l_member_variable_index, l_variable);
			
			return JS_INVALID_REFERENCE;
		}

		template<typename TM>
		JsValueRef CHAKRA_CALLBACK _js_getter(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			if (p_argument_count != 1)
			{
				_bc_log_wrong_number_of_arguments();
			}

			JsValueRef l_this = p_arguments[0];
			bc_script_getter l_external_getter = reinterpret_cast<bc_script_getter>(p_callback_state);
			bc_script_variable l_variable = (*l_external_getter)();
			
			return l_variable.get_platform_pack().m_js_value;
		}

		template<typename T, typename TM>
		JsValueRef CHAKRA_CALLBACK _js_object_getter(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			if (p_argument_count != 1)
			{
				_bc_log_wrong_number_of_arguments();
			}

			const JsValueRef l_this = p_arguments[0];
			bc_script_external_object<T>* l_external_object;

			bc_chakra_call(JsGetExternalData(l_this, &l_external_object)).throw_if_faild();
			bcUINT32 l_member_variable_getter_index = reinterpret_cast<bcUINT32>(p_callback_state);

			bc_script_variable l_member_variable = l_external_object->template call_member_function<0>(l_member_variable_getter_index);

			return l_member_variable.get_platform_pack().m_js_value;
		}

		template<typename TM>
		JsValueRef CHAKRA_CALLBACK _js_setter(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			if (p_argument_count != 2)
			{
				_bc_log_wrong_number_of_arguments();
			}

			bc_script_variable l_arg;

			JsValueRef l_this = p_arguments[0];
			l_arg.get_platform_pack().m_js_value = p_arguments[1];
			const bc_script_setter l_external_setter = reinterpret_cast<bc_script_setter>(p_callback_state);

			(*l_external_setter)(l_arg);

			return JS_INVALID_REFERENCE;
		}

		template<typename T, typename TM>
		JsValueRef CHAKRA_CALLBACK _js_object_setter(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			if (p_argument_count != 2)
			{
				_bc_log_wrong_number_of_arguments();
			}

			JsValueRef l_this;
			bc_script_variable l_arg;
			bc_script_external_object<T>* l_external_object;

			l_this = p_arguments[0];
			l_arg.get_platform_pack().m_js_value = p_arguments[1];
			bc_chakra_call(JsGetExternalData(l_this, &l_external_object)).throw_if_faild();
			bcUINT32 l_member_variable_setter_index = reinterpret_cast<bcUINT32>(p_callback_state);

			bc_script_arg_pack<1> l_args{ l_arg };
			l_external_object->template call_member_function<1>(l_member_variable_setter_index, l_args);

			return JS_INVALID_REFERENCE;
		}

		template<bcSIZE TArgCount>
		bc_script_variable _js_function_call(bc_script_free_function<TArgCount> p_external_function, bc_script_arg_pack<TArgCount>& p_pack)
		{
			const bc_script_variable l_call_result = bc_script_function::call_callback(p_external_function, p_pack);
			return l_call_result;
		}
		
		template<bcSIZE TArgCount>
		JsValueRef CHAKRA_CALLBACK _js_function(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			if (p_argument_count != TArgCount + 1) // first arg is this
			{
				_bc_log_wrong_number_of_arguments();
			}

			p_argument_count = std::min(static_cast<bcUINT16>(TArgCount + 1), p_argument_count);
			
			bc_script_arg_pack<TArgCount> l_pack;

			JsValueRef l_this = p_arguments[0];
			auto l_external_function = static_cast<bc_script_free_function<TArgCount>>(p_callback_state);

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

			bc_script_variable l_call_result = _js_function_call<TArgCount>(l_external_function, l_pack);

			return l_call_result.get_platform_pack().m_js_value;
		}

		template<typename T, bcSIZE TArgCount>
		bc_script_variable _js_object_function_call(bc_script_external_object<T>* p_external_object, bcUINT32 p_member_function_index, bc_script_arg_pack<TArgCount>& p_pack)
		{
			/*auto l_member_call_callable = [p_external_object, p_member_function_index](const auto&... p_args)
			{
				return p_external_object->template call_member_function<TArgCount>(p_member_function_index, p_args...);
			};

			const bc_script_variable l_call_result = bc_script_function::call_callback
			(
				l_member_call_callable,
				p_pack
			);

			return l_call_result;*/

			const bc_script_variable l_call_result = p_external_object->call_member_function(p_member_function_index, p_pack);
			return l_call_result;
		}
				
		template<typename T, bcSIZE TArgCount>
		JsValueRef CHAKRA_CALLBACK _js_object_function(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			if (p_argument_count != TArgCount + 1) // first arg is this
			{
				_bc_log_wrong_number_of_arguments();
			}

			p_argument_count = std::min(static_cast<bcUINT16>(TArgCount + 1), p_argument_count);
			
			JsValueRef l_this = p_arguments[0];
			bc_script_external_object<T>* l_external_object;
			bc_script_arg_pack<TArgCount> l_pack;

			bc_chakra_call(JsGetExternalData(l_this, reinterpret_cast<void**>(&l_external_object))).throw_if_faild();
			const bcUINT32 l_member_function_index = reinterpret_cast<bcUINT32>(p_callback_state);

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

			bc_script_variable l_call_result = _js_object_function_call<T, TArgCount>
			(
				l_external_object,
				l_member_function_index,
				l_pack
			);

			return l_call_result.get_platform_pack().m_js_value;
		}

		inline JsValueRef _create_js_descriptor(bc_script_context& p_context, bool p_writable, bool p_enumerable, bool p_configurable, JsValueRef p_value)
		{
			JsValueRef l_js_descriptor;

			JsCreateObject(&l_js_descriptor);

			JsValueRef l_writable;
			JsPropertyIdRef l_writable_property;
			JsBoolToBoolean(p_writable, &l_writable);
			JsGetPropertyIdFromName(L"writable", &l_writable_property);
			JsSetProperty(l_js_descriptor, l_writable_property, l_writable, true);

			JsValueRef l_enumerable;
			JsPropertyIdRef l_enumerable_property;
			JsBoolToBoolean(p_enumerable, &l_enumerable);
			JsGetPropertyIdFromName(L"enumerable", &l_enumerable_property);
			JsSetProperty(l_js_descriptor, l_enumerable_property, l_enumerable, true);

			JsValueRef l_configurable;
			JsPropertyIdRef l_configurable_property;
			JsBoolToBoolean(p_configurable, &l_configurable);
			JsGetPropertyIdFromName(L"configurable", &l_configurable_property);
			JsSetProperty(l_js_descriptor, l_configurable_property, l_configurable, true);

			JsValueRef l_value = p_value;
			JsPropertyIdRef l_value_property;
			JsGetPropertyIdFromName(L"value", &l_value_property);
			JsSetProperty(l_js_descriptor, l_value_property, l_value, true);

			return l_js_descriptor;
		}

		inline JsValueRef _create_js_descriptor(bc_script_context& p_context, bool p_writable, bool p_enumerable, bool p_configurable, JsValueRef p_getter, JsValueRef p_setter)
		{
			JsValueRef l_js_descriptor;

			JsCreateObject(&l_js_descriptor);

			/*JsValueRef l_writable;
			JsPropertyIdRef l_writable_property;
			JsBoolToBoolean(p_writable, &l_writable);
			JsGetPropertyIdFromName(L"writable", &l_writable_property);
			JsSetProperty(l_js_descriptor, l_writable_property, l_writable, true);*/

			JsValueRef l_enumerable;
			JsPropertyIdRef l_enumerable_property;
			JsBoolToBoolean(p_enumerable, &l_enumerable);
			JsGetPropertyIdFromName(L"enumerable", &l_enumerable_property);
			JsSetProperty(l_js_descriptor, l_enumerable_property, l_enumerable, true);

			JsValueRef l_configurable;
			JsPropertyIdRef l_configurable_property;
			JsBoolToBoolean(p_configurable, &l_configurable);
			JsGetPropertyIdFromName(L"configurable", &l_configurable_property);
			JsSetProperty(l_js_descriptor, l_configurable_property, l_configurable, true);

			JsPropertyIdRef l_getter_property;
			JsGetPropertyIdFromName(L"get", &l_getter_property);
			JsSetProperty(l_js_descriptor, l_getter_property, p_getter, true);

			if(p_writable)
			{
				JsPropertyIdRef l_setter_property;
				JsGetPropertyIdFromName(L"set", &l_setter_property);
				JsSetProperty(l_js_descriptor, l_setter_property, p_setter, true);
			}

			return l_js_descriptor;
		}

		template<typename TM>
		bool _create_js_property_constant(bc_script_context& p_context, JsValueRef p_js_prototype, const bcWCHAR* p_name, TM p_value)
		{
			bc_chakra_call l_call(p_context);
			bool l_succeeded = false;
			JsPropertyIdRef l_js_name;

			const JsValueRef l_value = p_context.create_variable(p_value).get_platform_pack().m_js_value;
			const JsValueRef l_js_descriptor = _create_js_descriptor(p_context, false, true, false, l_value);
			JsGetPropertyIdFromName(p_name, &l_js_name);

			l_call = JsDefineProperty(p_js_prototype, l_js_name, l_js_descriptor, &l_succeeded);
			//l_call = JsSetProperty(p_js_prototype, l_js_name, l_value.get_platform_pack().m_js_value, true);

			return l_succeeded;
		}

		template<typename TM>
		bool _create_js_property_default_get_set(bc_script_context& p_context, JsValueRef p_js_prototype, const bcWCHAR* p_name, TM* p_pointer)
		{
			bc_chakra_call l_call(p_context);
			bool l_succeeded = false;
			JsValueRef l_js_getter;
			JsValueRef l_js_setter;
			JsValueRef l_js_descriptor;
			JsPropertyIdRef l_js_name;

			l_call = JsCreateFunction(&_js_getter_default<TM>, reinterpret_cast<void*>(p_pointer), &l_js_getter);
			l_call = JsCreateFunction(&_js_setter_default<TM>, reinterpret_cast<void*>(p_pointer), &l_js_setter);
			l_js_descriptor = _create_js_descriptor(p_context, true, true, false, l_js_getter, l_js_setter);
			JsGetPropertyIdFromName(p_name, &l_js_name);

			l_call = JsDefineProperty(p_js_prototype, l_js_name, l_js_descriptor, &l_succeeded);

			return l_succeeded;
		}

		template<typename T, typename TM>
		bool _create_js_property_default_get_set(bc_script_context& p_context, JsValueRef p_js_prototype, const bcWCHAR* p_name, TM T::* p_pointer)
		{
			bc_chakra_call l_call(p_context);
			bool l_succeeded = false;
			JsValueRef l_js_getter;
			JsValueRef l_js_setter;
			JsValueRef l_js_descriptor;
			JsPropertyIdRef l_js_name;

			const bcUINT32 l_member_variable_ptr_index = bc_script_external_object<T>::set_member_variable_ptr(p_pointer);

			l_call = JsCreateFunction(&_js_object_getter_default<T, TM>, reinterpret_cast<void*>(l_member_variable_ptr_index), &l_js_getter);
			l_call = JsCreateFunction(&_js_object_setter_default<T, TM>, reinterpret_cast<void*>(l_member_variable_ptr_index), &l_js_setter);
			l_js_descriptor = _create_js_descriptor(p_context, true, true, false, l_js_getter, l_js_setter);
			JsGetPropertyIdFromName(p_name, &l_js_name);

			l_call = JsDefineProperty(p_js_prototype, l_js_name, l_js_descriptor, &l_succeeded);

			return l_succeeded;
		}

		template<typename TM>
		bool _create_js_property_get_set(bc_script_context& p_context, JsValueRef p_js_prototype, const bcWCHAR* p_name, bc_script_property_descriptor<TM>& p_descriptor)
		{
			bc_chakra_call l_call(p_context);
			bool l_succeeded = false;
			JsValueRef l_js_getter;
			JsValueRef l_js_setter;
			JsValueRef l_js_descriptor;
			JsPropertyIdRef l_js_name;

			if(p_descriptor.m_value)
			{
				l_call = JsCreateFunction(&_js_getter_default<TM>, reinterpret_cast<void*>(p_descriptor.m_value), &l_js_getter);
				l_call = JsCreateFunction(&_js_setter_default<TM>, reinterpret_cast<void*>(p_descriptor.m_value), &l_js_setter);
			}
			else
			{
				l_call = JsCreateFunction(&_js_getter<TM>, reinterpret_cast<void*>(p_descriptor.m_getter), &l_js_getter);
				l_call = JsCreateFunction(&_js_setter<TM>, reinterpret_cast<void*>(p_descriptor.m_setter), &l_js_setter);
			}

			l_js_descriptor = _create_js_descriptor(p_context, p_descriptor.m_writable, p_descriptor.m_enumerable, p_descriptor.m_configurable, l_js_getter, l_js_setter);
			JsGetPropertyIdFromName(p_name, &l_js_name);

			l_call = JsDefineProperty(p_js_prototype, l_js_name, l_js_descriptor, &l_succeeded);

			return l_succeeded;
		}

		template<typename T, typename TM>
		bool _create_js_property_get_set(bc_script_context& p_context, JsValueRef p_js_prototype, const bcWCHAR* p_name, bc_script_member_property_descriptor<T, TM>& p_descriptor)
		{
			bc_chakra_call l_call(p_context);
			bool l_succeeded = false;
			JsValueRef l_js_getter;
			JsValueRef l_js_setter;
			JsValueRef l_js_descriptor;
			JsPropertyIdRef l_js_name;
			
			if (p_descriptor.m_value)
			{
				const bcUINT32 l_member_variable_ptr_index = bc_script_external_object<T>::set_member_variable_ptr(p_descriptor.m_value);

				l_call = JsCreateFunction(&_js_object_getter_default<T, TM>, reinterpret_cast<void*>(l_member_variable_ptr_index), &l_js_getter);
				l_call = JsCreateFunction(&_js_object_setter_default<T, TM>, reinterpret_cast<void*>(l_member_variable_ptr_index), &l_js_setter);
			}
			else
			{
				const bcUINT32 l_member_variable_getter_ptr_index = bc_script_external_object<T>::set_member_function_ptr(p_descriptor.m_getter);
				const bcUINT32 l_member_variable_setter_ptr_index = bc_script_external_object<T>::set_member_function_ptr(p_descriptor.m_setter);

				l_call = JsCreateFunction(&_js_object_getter<T, TM>, reinterpret_cast<void*>(l_member_variable_getter_ptr_index), &l_js_getter);
				l_call = JsCreateFunction(&_js_object_setter<T, TM>, reinterpret_cast<void*>(l_member_variable_setter_ptr_index), &l_js_setter);
			}
			
			l_js_descriptor = _create_js_descriptor(p_context, true, true, false, l_js_getter, l_js_setter);
			JsGetPropertyIdFromName(p_name, &l_js_name);

			l_call = JsDefineProperty(p_js_prototype, l_js_name, l_js_descriptor, &l_succeeded);

			return l_succeeded;
		}

		template<bcSIZE TArgCount>
		bool _create_js_function(bc_script_context& p_context, JsValueRef p_js_prototype, const bcWCHAR* p_name, bc_script_free_function<TArgCount> p_func)
		{
			bc_chakra_call l_call(p_context);
			const bool l_succeeded = true;
			JsValueRef l_js_function;
			JsPropertyIdRef l_js_name;

			l_call = JsCreateFunction(&_js_function<TArgCount>, reinterpret_cast<void*>(p_func), &l_js_function);
			l_call = JsGetPropertyIdFromName(p_name, &l_js_name);

			l_call = JsSetProperty(p_js_prototype, l_js_name, l_js_function, true);

			return l_succeeded;
		}

		template<typename T, bcSIZE TArgCount>
		bool _create_js_object_function(bc_script_context& p_context, JsValueRef p_js_prototype, const bcWCHAR* p_name, bc_script_member_function<T, TArgCount> p_member_func)
		{
			bc_chakra_call l_call(p_context);
			const bool l_succeeded = true;
			JsValueRef l_js_function;
			JsPropertyIdRef l_js_name;

			const bcUINT32 l_member_function_ptr_index = bc_script_external_object<T>::template set_member_function_ptr<TArgCount>(p_member_func);

			l_call = JsCreateFunction(&_js_object_function<T, TArgCount>, reinterpret_cast<void*>(l_member_function_ptr_index), &l_js_function);
			l_call = JsGetPropertyIdFromName(p_name, &l_js_name);

			l_call = JsSetProperty(p_js_prototype, l_js_name, l_js_function, true);

			return l_succeeded;
		}

		template<typename T>
		struct bc_platform_script_prototype_builder_pack<platform::g_api_win32, T>
		{
			JsValueRef m_js_ctor_function;
			JsValueRef m_js_prototype;
		};

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>::bc_platform_script_prototype_builder(bc_script_context& p_context)
			: m_context(p_context)
		{
			bc_chakra_call l_call(p_context);
			m_pack.m_js_ctor_function = JS_INVALID_REFERENCE;
			
			l_call = JsCreateObject(&m_pack.m_js_prototype);
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>::bc_platform_script_prototype_builder(bc_platform_script_prototype_builder&& p_other) noexcept
			: m_context(p_other.m_context)
		{
			operator=(std::move(p_other));
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>::~bc_platform_script_prototype_builder() = default;

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>& bc_platform_script_prototype_builder<TPlatform, T>::operator=(bc_platform_script_prototype_builder&& p_other) noexcept
		{
			m_pack.m_js_ctor_function = p_other.m_pack.m_js_ctor_function;
			m_pack.m_js_prototype = p_other.m_pack.m_js_prototype;

			p_other.m_pack.m_js_ctor_function = JS_INVALID_REFERENCE;
			p_other.m_pack.m_js_prototype = JS_INVALID_REFERENCE;

			return *this;
		}

		template<platform::bc_platform TPlatform, typename T>
		template<bcSIZE TArgCount>
		bc_platform_script_prototype_builder<TPlatform, T>& bc_platform_script_prototype_builder<TPlatform, T>::constructor()
		{
			if(m_pack.m_js_ctor_function != JS_INVALID_REFERENCE)
			{
				throw bc_invalid_operation_exception("Constructor signature has already defined");
			}

			bc_chakra_call l_call = JsCreateFunction(&_js_object_ctor<T, TArgCount>, nullptr, &m_pack.m_js_ctor_function);

			return *this;
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>& bc_platform_script_prototype_builder<TPlatform, T>::constant(const bcWCHAR* p_name, bc_script_bool p_bool)
		{
			const bool l_succeeded = _create_js_property_constant(m_context, m_pack.m_js_prototype, p_name, p_bool);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>& bc_platform_script_prototype_builder<TPlatform, T>::constant(const bcWCHAR* p_name, bc_script_int p_integer)
		{
			const bool l_succeeded = _create_js_property_constant(m_context, m_pack.m_js_prototype, p_name, p_integer);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>& bc_platform_script_prototype_builder<TPlatform, T>::constant(const bcWCHAR* p_name, bc_script_double p_double)
		{
			const bool l_succeeded = _create_js_property_constant(m_context, m_pack.m_js_prototype, p_name, p_double);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>& bc_platform_script_prototype_builder<TPlatform, T>::constant(const bcWCHAR* p_name, bc_script_string& p_string)
		{
			const bool l_succeeded = _create_js_property_constant(m_context, m_pack.m_js_prototype, p_name, p_string);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>& bc_platform_script_prototype_builder<TPlatform, T>::property(const bcWCHAR* p_name, bc_script_bool type::* p_bool)
		{
			const bool l_succeeded = _create_js_property_default_get_set(m_context, m_pack.m_js_prototype, p_name, p_bool);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>& bc_platform_script_prototype_builder<TPlatform, T>::property(const bcWCHAR* p_name, bc_script_int type::* p_int)
		{
			const bool l_succeeded = _create_js_property_default_get_set(m_context, m_pack.m_js_prototype, p_name, p_int);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>& bc_platform_script_prototype_builder<TPlatform, T>::property(const bcWCHAR* p_name, bc_script_double type::* p_double)
		{
			const bool l_succeeded = _create_js_property_default_get_set(m_context, m_pack.m_js_prototype, p_name, p_double);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>& bc_platform_script_prototype_builder<TPlatform, T>::property(const bcWCHAR* p_name, bc_script_string type::* p_string)
		{
			const bool l_succeeded = _create_js_property_default_get_set(m_context, m_pack.m_js_prototype, p_name, p_string);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>& bc_platform_script_prototype_builder<TPlatform, T>::property(const bcWCHAR* p_name, bc_script_object type::* p_object)
		{
			const bool l_succeeded = _create_js_property_default_get_set(m_context, m_pack.m_js_prototype, p_name, p_object);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>& bc_platform_script_prototype_builder<TPlatform, T>::property(const bcWCHAR* p_name, bc_script_member_property_descriptor<type, bc_script_bool>& p_descriptor)
		{
			const bool l_succeeded = _create_js_property_get_set(m_context, m_pack.m_js_prototype, p_name, p_descriptor);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>& bc_platform_script_prototype_builder<TPlatform, T>::property(const bcWCHAR* p_name, bc_script_member_property_descriptor<type, bc_script_int>& p_descriptor)
		{
			const bool l_succeeded = _create_js_property_get_set(m_context, m_pack.m_js_prototype, p_name, p_descriptor);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>& bc_platform_script_prototype_builder<TPlatform, T>::property(const bcWCHAR* p_name, bc_script_member_property_descriptor<type, bc_script_double>& p_descriptor)
		{
			const bool l_succeeded = _create_js_property_get_set(m_context, m_pack.m_js_prototype, p_name, p_descriptor);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>& bc_platform_script_prototype_builder<TPlatform, T>::property(const bcWCHAR* p_name, bc_script_member_property_descriptor<type, bc_script_string>& p_descriptor)
		{
			const bool l_succeeded = _create_js_property_get_set(m_context, m_pack.m_js_prototype, p_name, p_descriptor);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_prototype_builder<TPlatform, T>& bc_platform_script_prototype_builder<TPlatform, T>::property(const bcWCHAR* p_name, bc_script_member_property_descriptor<type, bc_script_object>& p_descriptor)
		{
			const bool l_succeeded = _create_js_property_get_set(m_context, m_pack.m_js_prototype, p_name, p_descriptor);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<platform::bc_platform TPlatform, typename T>
		template<typename TR, typename ...TA>
		bc_platform_script_prototype_builder<TPlatform, T>& bc_platform_script_prototype_builder<TPlatform, T>::function(const bcWCHAR* p_name, TR(T::* p_member_func)(const TA&...))
		{
			static_assert(std::is_same_v<bc_script_variable, std::decay_t<TR>>, "function return type must be bc_script_variable");
			static_assert(std::conjunction_v<std::is_same<bc_script_variable, std::decay_t<TA>>...>, "function argument types must be bc_script_variable");
			
			const bool l_succeeded = _create_js_object_function<T, sizeof...(TA)>(m_context, m_pack.m_js_prototype, p_name, p_member_func);

			BC_ASSERT(l_succeeded);

			return *this;
		}
	}
}