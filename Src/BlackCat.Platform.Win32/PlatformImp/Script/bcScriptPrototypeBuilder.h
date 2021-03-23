// [10/13/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptContext.h"
#include "PlatformImp/Script/bcScriptObject.h"

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
		template< typename T >
		void CHAKRA_CALLBACK _js_object_ctor_finalizer(void* p_object)
		{
			bc_script_runtime::destroy_native(reinterpret_cast<bc_script_external_object< T >*>(p_object));
		}

		template< typename T, typename ...TA >
		JsValueRef CHAKRA_CALLBACK _js_object_ctor(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			BC_ASSERT(p_is_construct_call);
			// TODO Make a chiose for more and less argument count than expected
			BC_ASSERT(p_argument_count == sizeof...(TA) + 1);

			bc_chakra_call l_call;
			bc_script_var_pack< TA... > l_pack;
			bc_script_external_object< T >* l_native_object;
			JsValueRef l_script_object;

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

			l_native_object = bc_script_function< bc_script_external_object< T >*(TA ...) >::_call_callback
			(
				&bc_script_runtime::create_native< T >,
				l_pack.data(),
				l_pack.size()
			);

			l_call = JsCreateExternalObject(l_native_object, &_js_object_ctor_finalizer<T>, &l_script_object);
			l_call = JsSetPrototype(l_script_object, l_native_object->get_prototype().get_platform_pack().m_js_prototype);

			return l_script_object;
		}

		template< typename TM >
		JsValueRef CHAKRA_CALLBACK _js_getter_default(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			// TODO Make a chiose for more and less argument count than expected
			BC_ASSERT(p_argument_count == 1);

			JsValueRef l_this;
			TM* l_external_variable;

			l_this = p_arguments[0];
			l_external_variable = reinterpret_cast<TM*>(p_callback_state);

			bc_script_variable l_variable = bc_script_variable::_pack_arg(*l_external_variable);

			return l_variable.get_platform_pack().m_js_value;
		}

		template< typename T, typename TM >
		JsValueRef CHAKRA_CALLBACK _js_object_getter_default(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			// TODO Make a chiose for more and less argument count than expected
			BC_ASSERT(p_argument_count == 1);

			bc_chakra_call l_call;
			JsValueRef l_this = p_arguments[0];
			bc_script_external_object< T >* l_external_object;
			bcUINT32 l_member_variable_index;

			l_call = JsGetExternalData(l_this, reinterpret_cast< void** >(&l_external_object));
			l_call.throw_if_faild();
			l_member_variable_index = reinterpret_cast<bcUINT32>(p_callback_state);

			TM l_member_variable = l_external_object->get_member_variable<TM>(l_member_variable_index);
			bc_script_variable l_variable = bc_script_variable::_pack_arg(l_member_variable);

			return l_variable.get_platform_pack().m_js_value;
		}

		template< typename TM >
		JsValueRef CHAKRA_CALLBACK _js_setter_default(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			// TODO Make a chiose for more and less argument count than expected
			BC_ASSERT(p_argument_count == 2);

			JsValueRef l_this;
			bc_script_variable l_arg;
			TM* l_external_variable;

			l_this = p_arguments[0];
			l_arg.get_platform_pack().m_js_value = p_arguments[1];
			l_external_variable = reinterpret_cast<TM*>(p_callback_state);

			TM l_variable;
			bc_script_variable::_unpack_arg(l_arg, &l_variable);

			*l_external_variable = l_variable;

			return JS_INVALID_REFERENCE;
		}

		template< typename T, typename TM >
		JsValueRef CHAKRA_CALLBACK _js_object_setter_default(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			// TODO Make a chiose for more and less argument count than expected
			BC_ASSERT(p_argument_count == 2);

			bc_chakra_call l_call;
			JsValueRef l_this;
			bc_script_variable l_arg;
			bc_script_external_object< T >* l_external_object;
			bcUINT32 l_member_variable_index;

			l_this = p_arguments[0];
			l_arg.get_platform_pack().m_js_value = p_arguments[1];
			l_call = JsGetExternalData(l_this, reinterpret_cast< void** >(&l_external_object));
			l_call.throw_if_faild();
			l_member_variable_index = reinterpret_cast<bcUINT32>(p_callback_state);

			TM l_variable;
			bc_script_variable::_unpack_arg(l_arg, &l_variable);
			l_external_object->set_member_variable<TM>(l_member_variable_index, l_variable);
			
			return JS_INVALID_REFERENCE;
		}

		template< typename TM >
		JsValueRef CHAKRA_CALLBACK _js_getter(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			// TODO Make a chiose for more and less argument count than expected
			BC_ASSERT(p_argument_count == 1);

			using getter_t = bc_script_getter<TM>;

			bc_chakra_call l_call;
			JsValueRef l_this = p_arguments[0];
			getter_t l_external_getter;

			l_external_getter = reinterpret_cast<getter_t>(p_callback_state);

			TM l_variable = (*l_external_getter)();
			bc_script_variable l_packed_variable = bc_script_variable::_pack_arg(l_variable);

			return l_packed_variable.get_platform_pack().m_js_value;
		}

		template< typename T, typename TM >
		JsValueRef CHAKRA_CALLBACK _js_object_getter(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			// TODO Make a chiose for more and less argument count than expected
			BC_ASSERT(p_argument_count == 1);

			bc_chakra_call l_call;
			JsValueRef l_this = p_arguments[0];
			bc_script_external_object< T >* l_external_object;
			bcUINT32 l_member_variable_getter_index;

			l_call = JsGetExternalData(l_this, &l_external_object);
			l_call.throw_if_faild();
			l_member_variable_getter_index = reinterpret_cast<bcUINT32>(p_callback_state);

			TM l_member_variable = l_external_object->call_member_function<TM, void>(l_member_variable_getter_index);
			bc_script_variable l_variable = bc_script_variable::_pack_arg(l_member_variable);

			return l_variable.get_platform_pack().m_js_value;
		}

		template< typename TM >
		JsValueRef CHAKRA_CALLBACK _js_setter(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			// TODO Make a chiose for more and less argument count than expected
			BC_ASSERT(p_argument_count == 2);

			using setter_t = bc_script_setter<TM>;

			JsValueRef l_this;
			bc_script_variable l_arg;
			setter_t l_external_setter;

			l_this = p_arguments[0];
			l_arg.get_platform_pack().m_js_value = p_arguments[1];
			l_external_setter = reinterpret_cast<setter_t>(p_callback_state);

			TM l_variable;
			bc_script_variable::_unpack_arg(l_arg, &l_variable);
			(*l_external_setter)(l_variable);

			return JS_INVALID_REFERENCE;
		}

		template< typename T, typename TM >
		JsValueRef CHAKRA_CALLBACK _js_object_setter(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			// TODO Make a chiose for more and less argument count than expected
			BC_ASSERT(p_argument_count == 2);

			bc_chakra_call l_call;
			JsValueRef l_this;
			bc_script_variable l_arg;
			bc_script_external_object< T >* l_external_object;
			bcUINT32 l_member_variable_setter_index;

			l_this = p_arguments[0];
			l_arg.get_platform_pack().m_js_value = p_arguments[1];
			l_call = JsGetExternalData(l_this, &l_external_object);
			l_call.throw_if_faild();
			l_member_variable_setter_index = reinterpret_cast<bcUINT32>(p_callback_state);

			TM l_variable;
			bc_script_variable::_unpack_arg(l_arg, &l_variable);
			l_external_object->call_member_function<void, TM>(l_member_variable_setter_index, l_variable);

			return JS_INVALID_REFERENCE;
		}

		template< typename TR, typename ...TA >
		JsValueRef CHAKRA_CALLBACK _js_function(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			// TODO Make a chiose for more and less argument count than expected
			BC_ASSERT(p_argument_count == sizeof...(TA) + 1);

			using function_t = bc_script_free_function<TR, TA...>;

			JsValueRef l_this;
			function_t l_external_function;
			bc_script_var_pack< TA... > l_pack;

			l_this = p_arguments[0];
			l_external_function = reinterpret_cast<function_t>(p_callback_state);

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

			bc_script_variable l_call_result = _js_function_call< TR, TA... >
			(
				std::is_same< TR, void >::type(),
				l_external_function,
				l_pack
			);

			return l_call_result.get_platform_pack().m_js_value;
		}

		template< typename TR, typename ...TA >
		bc_script_variable _js_function_call(std::false_type, bc_script_free_function<TR, TA...> p_external_function, bc_script_var_pack< TA... >& p_pack)
		{
			auto l_call_callable = [p_external_function](const TA&... p_args)
			{
				return (*p_external_function)(p_args...);
			};

			TR l_call_result = bc_script_function< TR(TA ...) >::_call_callback
			(
				l_call_callable,
				p_pack.data(),
				p_pack.size()
			);

			bc_script_variable l_variable = bc_script_variable::_pack_arg(l_call_result);

			return l_variable;
		}

		template< typename TR, typename ...TA >
		bc_script_variable _js_function_call(std::true_type, bc_script_free_function<TR, TA...> p_external_function, bc_script_var_pack< TA... >& p_pack)
		{
			auto l_call_callable = [p_external_function](const TA&... p_args)
			{
				return (*p_external_function)(p_args...);
			};

			bc_script_function< TR(TA ...) >::_call_callback
			(
				l_call_callable,
				p_pack.data(),
				p_pack.size()
			);

			bc_script_variable l_variable = bc_script_variable::_pack_arg();

			return l_variable;
		}

		template< typename T, typename TR, typename ...TA >
		JsValueRef CHAKRA_CALLBACK _js_object_function(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			// TODO Make a chiose for more and less argument count than expected
			BC_ASSERT(p_argument_count == sizeof...(TA) + 1);

			bc_chakra_call l_call;
			JsValueRef l_this = p_arguments[0];
			bc_script_external_object< T >* l_external_object;
			bcUINT32 l_member_function_index;
			bc_script_var_pack< TA... > l_pack;

			l_call = JsGetExternalData(l_this, reinterpret_cast< void** >(&l_external_object));
			l_call.throw_if_faild();
			l_member_function_index = reinterpret_cast<bcUINT32>(p_callback_state);

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

			bc_script_variable l_call_result = _js_object_function_call< T, TR, TA... >
			(
				std::is_same< TR, void >::type(),
				l_external_object,
				l_member_function_index,
				l_pack
			);

			return l_call_result.get_platform_pack().m_js_value;
		}

		template< typename T, typename TR, typename ...TA >
		bc_script_variable _js_object_function_call(std::false_type, bc_script_external_object< T >* p_external_object, bcUINT32 p_member_function_index, bc_script_var_pack< TA... >& p_pack)
		{
			auto l_member_call_callable = [p_external_object, p_member_function_index](const TA&... p_args)
			{
				return p_external_object->call_member_function< TR, TA... >(p_member_function_index, p_args...);
			};

			TR l_call_result = bc_script_function< TR(TA ...) >::_call_callback
			(
				l_member_call_callable,
				p_pack.data(),
				p_pack.size()
			);

			bc_script_variable l_variable = bc_script_variable::_pack_arg(l_call_result);

			return l_variable;
		}

		template< typename T, typename TR, typename ...TA >
		bc_script_variable _js_object_function_call(std::true_type, bc_script_external_object< T >* p_external_object, bcUINT32 p_member_function_index, bc_script_var_pack< TA... >& p_pack)
		{
			auto l_member_call_callable = [p_external_object, p_member_function_index](const TA&... p_args)
				{
					return p_external_object->call_member_function< TR, TA... >(p_member_function_index, p_args...);
				};

			bc_script_function< TR(TA ...) >::_call_callback
			(
				l_member_call_callable,
				p_pack.data(),
				p_pack.size()
			);

			bc_script_variable l_variable = bc_script_variable::_pack_arg();

			return l_variable;
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

		template< typename TM >
		bool _create_js_property_constant(bc_script_context& p_context, JsValueRef p_js_prototype, const bcWCHAR* p_name, TM p_value)
		{
			bc_chakra_call l_call(p_context);
			bool l_successed = false;
			JsValueRef l_value;
			JsValueRef l_js_descriptor;
			JsPropertyIdRef l_js_name;

			l_value = p_context.create_variable(p_value).get_platform_pack().m_js_value;
			l_js_descriptor = _create_js_descriptor(p_context, false, true, false, l_value);
			JsGetPropertyIdFromName(p_name, &l_js_name);

			l_call = JsDefineProperty(p_js_prototype, l_js_name, l_js_descriptor, &l_successed);
			//l_call = JsSetProperty(p_js_prototype, l_js_name, l_value.get_platform_pack().m_js_value, true);

			return l_successed;
		}

		template< typename TM >
		bool _create_js_property_default_get_set(bc_script_context& p_context, JsValueRef p_js_prototype, const bcWCHAR* p_name, TM* p_pointer)
		{
			bc_chakra_call l_call(p_context);
			bool l_successed = false;
			JsValueRef l_js_getter;
			JsValueRef l_js_setter;
			JsValueRef l_js_descriptor;
			JsPropertyIdRef l_js_name;

			l_call = JsCreateFunction(&_js_getter_default<TM>, reinterpret_cast< void* >(p_pointer), &l_js_getter);
			l_call = JsCreateFunction(&_js_setter_default<TM>, reinterpret_cast< void* >(p_pointer), &l_js_setter);
			l_js_descriptor = _create_js_descriptor(p_context, true, true, false, l_js_getter, l_js_setter);
			JsGetPropertyIdFromName(p_name, &l_js_name);

			l_call = JsDefineProperty(p_js_prototype, l_js_name, l_js_descriptor, &l_successed);

			return l_successed;
		}

		template< typename T, typename TM >
		bool _create_js_property_default_get_set(bc_script_context& p_context, JsValueRef p_js_prototype, const bcWCHAR* p_name, TM T::* p_pointer)
		{
			bc_chakra_call l_call(p_context);
			bool l_successed = false;
			JsValueRef l_js_getter;
			JsValueRef l_js_setter;
			JsValueRef l_js_descriptor;
			JsPropertyIdRef l_js_name;

			bcUINT32 l_member_variable_ptr_index = bc_script_external_object<T>::set_member_variable_ptr(p_pointer);

			l_call = JsCreateFunction(&_js_object_getter_default<T, TM>, reinterpret_cast< void* >(l_member_variable_ptr_index), &l_js_getter);
			l_call = JsCreateFunction(&_js_object_setter_default<T, TM>, reinterpret_cast< void* >(l_member_variable_ptr_index), &l_js_setter);
			l_js_descriptor = _create_js_descriptor(p_context, true, true, false, l_js_getter, l_js_setter);
			JsGetPropertyIdFromName(p_name, &l_js_name);

			l_call = JsDefineProperty(p_js_prototype, l_js_name, l_js_descriptor, &l_successed);

			return l_successed;
		}

		template< typename TM >
		bool _create_js_property_get_set(bc_script_context& p_context, JsValueRef p_js_prototype, const bcWCHAR* p_name, bc_script_property_descriptor<TM>& p_descriptor)
		{
			bc_chakra_call l_call(p_context);
			bool l_successed = false;
			JsValueRef l_js_getter;
			JsValueRef l_js_setter;
			JsValueRef l_js_descriptor;
			JsPropertyIdRef l_js_name;

			if(p_descriptor.m_value)
			{
				l_call = JsCreateFunction(&_js_getter_default<TM>, reinterpret_cast< void* >(p_descriptor.m_value), &l_js_getter);
				l_call = JsCreateFunction(&_js_setter_default<TM>, reinterpret_cast< void* >(p_descriptor.m_value), &l_js_setter);
			}
			else
			{
				l_call = JsCreateFunction(&_js_getter<TM>, reinterpret_cast< void* >(p_descriptor.m_getter), &l_js_getter);
				l_call = JsCreateFunction(&_js_setter<TM>, reinterpret_cast< void* >(p_descriptor.m_setter), &l_js_setter);
			}

			l_js_descriptor = _create_js_descriptor(p_context, p_descriptor.m_writable, p_descriptor.m_enumerable, p_descriptor.m_configurable, l_js_getter, l_js_setter);
			JsGetPropertyIdFromName(p_name, &l_js_name);

			l_call = JsDefineProperty(p_js_prototype, l_js_name, l_js_descriptor, &l_successed);

			return l_successed;
		}

		template< typename T, typename TM >
		bool _create_js_property_get_set(bc_script_context& p_context, JsValueRef p_js_prototype, const bcWCHAR* p_name, bc_script_member_property_descriptor<T, TM>& p_descriptor)
		{
			bc_chakra_call l_call(p_context);
			bool l_successed = false;
			JsValueRef l_js_getter;
			JsValueRef l_js_setter;
			JsValueRef l_js_descriptor;
			JsPropertyIdRef l_js_name;
			
			if (p_descriptor.m_value)
			{
				bcUINT32 l_member_variable_ptr_index = bc_script_external_object<T>::set_member_variable_ptr(p_descriptor.m_value);

				l_call = JsCreateFunction(&_js_object_getter_default<T, TM>, reinterpret_cast< void* >(l_member_variable_ptr_index), &l_js_getter);
				l_call = JsCreateFunction(&_js_object_setter_default<T, TM>, reinterpret_cast< void* >(l_member_variable_ptr_index), &l_js_setter);
			}
			else
			{
				bcUINT32 l_member_variable_getter_ptr_index = bc_script_external_object<T>::set_member_function_ptr(p_descriptor.m_getter);
				bcUINT32 l_member_variable_setter_ptr_index = bc_script_external_object<T>::set_member_function_ptr(p_descriptor.m_setter);

				l_call = JsCreateFunction(&_js_object_getter<T, TM>, reinterpret_cast< void* >(l_member_variable_getter_ptr_index), &l_js_getter);
				l_call = JsCreateFunction(&_js_object_setter<T, TM>, reinterpret_cast< void* >(l_member_variable_setter_ptr_index), &l_js_setter);
			}
			
			l_js_descriptor = _create_js_descriptor(p_context, true, true, false, l_js_getter, l_js_setter);
			JsGetPropertyIdFromName(p_name, &l_js_name);

			l_call = JsDefineProperty(p_js_prototype, l_js_name, l_js_descriptor, &l_successed);

			return l_successed;
		}

		template< typename TR, typename ...TA >
		bool _create_js_function(bc_script_context& p_context, JsValueRef p_js_prototype, const bcWCHAR* p_name, bc_script_free_function< TR, TA... > p_func)
		{
			bc_chakra_call l_call(p_context);
			const bool l_successed = true;
			JsValueRef l_js_function;
			JsPropertyIdRef l_js_name;

			l_call = JsCreateFunction(&_js_function< TR, TA... >, reinterpret_cast< void* >(p_func), &l_js_function);
			l_call = JsGetPropertyIdFromName(p_name, &l_js_name);

			l_call = JsSetProperty(p_js_prototype, l_js_name, l_js_function, true);

			return l_successed;
		}

		template< typename T, typename TR, typename ...TA >
		bool _create_js_function(bc_script_context& p_context, JsValueRef p_js_prototype, const bcWCHAR* p_name, bc_script_memeber_function<T, TR, TA...> p_member_func)
		{
			bc_chakra_call l_call(p_context);
			const bool l_successed = true;
			JsValueRef l_js_function;
			JsPropertyIdRef l_js_name;

			bcUINT32 l_member_function_ptr_index = bc_script_external_object<T>::set_member_function_ptr(p_member_func);

			l_call = JsCreateFunction(&_js_object_function<T, TR, TA...>, reinterpret_cast< void* >(l_member_function_ptr_index), &l_js_function);
			l_call = JsGetPropertyIdFromName(p_name, &l_js_name);

			l_call = JsSetProperty(p_js_prototype, l_js_name, l_js_function, true);

			return l_successed;
		}

		template< typename T >
		struct bc_platform_script_prototype_builder_pack< core_platform::g_api_win32, T >
		{
			JsValueRef m_js_ctor_function;
			JsValueRef m_js_prototype;
		};

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >::bc_platform_script_prototype_builder(bc_script_context& p_context)
			: m_context(p_context)
		{
			bc_chakra_call l_call(p_context);
			m_pack.m_js_ctor_function = JS_INVALID_REFERENCE;
			
			l_call = JsCreateObject(&m_pack.m_js_prototype);
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >::bc_platform_script_prototype_builder(bc_platform_script_prototype_builder&& p_other) noexcept
			: m_context(p_other.m_context)
		{
			operator=(std::move(p_other));
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >::~bc_platform_script_prototype_builder() = default;

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >& bc_platform_script_prototype_builder< TPlatform, T >::operator=(bc_platform_script_prototype_builder&& p_other) noexcept
		{
			m_pack.m_js_ctor_function = p_other.m_pack.m_js_ctor_function;
			m_pack.m_js_prototype = p_other.m_pack.m_js_prototype;

			p_other.m_pack.m_js_ctor_function = JS_INVALID_REFERENCE;
			p_other.m_pack.m_js_prototype = JS_INVALID_REFERENCE;

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		template< typename ...TA >
		bc_platform_script_prototype_builder< TPlatform, T >& bc_platform_script_prototype_builder< TPlatform, T >::constructor()
		{
			if(m_pack.m_js_ctor_function != JS_INVALID_REFERENCE)
			{
				throw bc_invalid_operation_exception("Constructor signature has already defined");
			}

			bc_chakra_call l_call = JsCreateFunction(&_js_object_ctor<T, TA...>, nullptr, &m_pack.m_js_ctor_function);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >& bc_platform_script_prototype_builder< TPlatform, T >::constant(const bcWCHAR* p_name, bc_script_bool p_bool)
		{
			const bool l_successed = _create_js_property_constant(m_context, m_pack.m_js_prototype, p_name, p_bool);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >& bc_platform_script_prototype_builder< TPlatform, T >::constant(const bcWCHAR* p_name, bc_script_int p_integer)
		{
			const bool l_successed = _create_js_property_constant(m_context, m_pack.m_js_prototype, p_name, p_integer);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >& bc_platform_script_prototype_builder< TPlatform, T >::constant(const bcWCHAR* p_name, bc_script_double p_double)
		{
			const bool l_successed = _create_js_property_constant(m_context, m_pack.m_js_prototype, p_name, p_double);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >& bc_platform_script_prototype_builder< TPlatform, T >::constant(const bcWCHAR* p_name, bc_script_string& p_string)
		{
			const bool l_successed = _create_js_property_constant(m_context, m_pack.m_js_prototype, p_name, p_string);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >& bc_platform_script_prototype_builder< TPlatform, T >::property(const bcWCHAR* p_name, bc_script_bool type::* p_bool)
		{
			const bool l_successed = _create_js_property_default_get_set(m_context, m_pack.m_js_prototype, p_name, p_bool);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >& bc_platform_script_prototype_builder< TPlatform, T >::property(const bcWCHAR* p_name, bc_script_int type::* p_int)
		{
			const bool l_successed = _create_js_property_default_get_set(m_context, m_pack.m_js_prototype, p_name, p_int);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >& bc_platform_script_prototype_builder< TPlatform, T >::property(const bcWCHAR* p_name, bc_script_double type::* p_double)
		{
			const bool l_successed = _create_js_property_default_get_set(m_context, m_pack.m_js_prototype, p_name, p_double);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >& bc_platform_script_prototype_builder< TPlatform, T >::property(const bcWCHAR* p_name, bc_script_string type::* p_string)
		{
			const bool l_successed = _create_js_property_default_get_set(m_context, m_pack.m_js_prototype, p_name, p_string);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >& bc_platform_script_prototype_builder< TPlatform, T >::property(const bcWCHAR* p_name, bc_script_object type::* p_object)
		{
			const bool l_successed = _create_js_property_default_get_set(m_context, m_pack.m_js_prototype, p_name, p_object);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >& bc_platform_script_prototype_builder< TPlatform, T >::property(const bcWCHAR* p_name, bc_script_member_property_descriptor< type, bc_script_bool >& p_descriptor)
		{
			const bool l_successed = _create_js_property_get_set(m_context, m_pack.m_js_prototype, p_name, p_descriptor);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >& bc_platform_script_prototype_builder< TPlatform, T >::property(const bcWCHAR* p_name, bc_script_member_property_descriptor< type, bc_script_int >& p_descriptor)
		{
			const bool l_successed = _create_js_property_get_set(m_context, m_pack.m_js_prototype, p_name, p_descriptor);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >& bc_platform_script_prototype_builder< TPlatform, T >::property(const bcWCHAR* p_name, bc_script_member_property_descriptor< type, bc_script_double >& p_descriptor)
		{
			const bool l_successed = _create_js_property_get_set(m_context, m_pack.m_js_prototype, p_name, p_descriptor);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >& bc_platform_script_prototype_builder< TPlatform, T >::property(const bcWCHAR* p_name, bc_script_member_property_descriptor< type, bc_script_string >& p_descriptor)
		{
			const bool l_successed = _create_js_property_get_set(m_context, m_pack.m_js_prototype, p_name, p_descriptor);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_prototype_builder< TPlatform, T >& bc_platform_script_prototype_builder< TPlatform, T >::property(const bcWCHAR* p_name, bc_script_member_property_descriptor< type, bc_script_object >& p_descriptor)
		{
			const bool l_successed = _create_js_property_get_set(m_context, m_pack.m_js_prototype, p_name, p_descriptor);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		template< typename TR, typename ... TA >
		bc_platform_script_prototype_builder< TPlatform, T >& bc_platform_script_prototype_builder< TPlatform, T >::function(const bcWCHAR* p_name, bc_script_memeber_function<type, TR, TA...> p_member_func)
		{
			const bool l_successed = _create_js_function(m_context, m_pack.m_js_prototype, p_name, p_member_func);

			BC_ASSERT(l_successed);

			return *this;
		}
	}
}