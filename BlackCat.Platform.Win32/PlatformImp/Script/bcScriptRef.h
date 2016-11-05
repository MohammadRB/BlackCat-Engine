// [10/11/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptReference.h"

#include "Platform/Script/bcScriptRef.h"
#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/Script/bcScriptContext.h"
#include "PlatformImp/Script/bcScriptVariable.h"
#include "PlatformImp/Script/bcScriptByteCode.h"
#include "PlatformImp/Script/bcScriptObject.h"
#include "PlatformImp/Script/bcScriptString.h"
#include "PlatformImp/Script/bcScriptFunction.h"
#include "PlatformImp/Script/bcScriptError.h"

namespace black_cat
{
	namespace platform
	{
		inline void _inc_ref(bc_script_context& p_context)
		{
			bc_chakra_call l_call = JsAddRef(p_context.get_platform_pack().m_js_context, nullptr);
		}

		inline void _dec_ref(bc_script_context& p_context)
		{
			bc_chakra_call l_call = JsRelease(p_context.get_platform_pack().m_js_context, nullptr);
		}

		inline void _inc_ref(bc_script_bytecode& p_byte_code)
		{
			bc_chakra_call l_call = JsAddRef(p_byte_code.get_platform_pack().m_js_parsed_script, nullptr);
		}

		inline void _dec_ref(bc_script_bytecode& p_byte_code)
		{
			bc_chakra_call l_call = JsRelease(p_byte_code.get_platform_pack().m_js_parsed_script, nullptr);
		}

		inline void _inc_ref(bc_script_variable& p_variable)
		{
			bc_chakra_call l_call = JsAddRef(p_variable.get_platform_pack().m_js_value, nullptr);
		}

		inline void _dec_ref(bc_script_variable& p_variable)
		{
			bc_chakra_call l_call = JsRelease(p_variable.get_platform_pack().m_js_value, nullptr);
		}

		inline void _inc_ref(bc_script_object& p_object)
		{
			bc_chakra_call l_call = JsAddRef(p_object.get_platform_pack().m_js_object, nullptr);
		}

		inline void _dec_ref(bc_script_object& p_object)
		{
			bc_chakra_call l_call = JsRelease(p_object.get_platform_pack().m_js_object, nullptr);
		}

		inline void _inc_ref(bc_script_string& p_string)
		{
			bc_chakra_call l_call = JsAddRef(p_string.get_platform_pack().m_js_string, nullptr);
		}

		inline void _dec_ref(bc_script_string& p_string)
		{
			bc_chakra_call l_call = JsRelease(p_string.get_platform_pack().m_js_string, nullptr);
		}

		template< typename TR, typename ...TA >
		inline void _inc_ref(bc_script_function< TR(TA ...) >& p_function)
		{
			bc_chakra_call l_call = JsAddRef(p_function.get_platform_pack().m_js_function, nullptr);
		}

		template< typename TR, typename ...TA >
		inline void _dec_ref(bc_script_function< TR(TA ...) >& p_function)
		{
			bc_chakra_call l_call = JsRelease(p_function.get_platform_pack().m_js_function, nullptr);
		}

		inline void _inc_ref(bc_script_error& p_error)
		{
			bc_chakra_call l_call = JsAddRef(p_error.get_platform_pack().m_js_error, nullptr);
		}

		inline void _dec_ref(bc_script_error& p_error)
		{
			bc_chakra_call l_call = JsRelease(p_error.get_platform_pack().m_js_error, nullptr);
		}

		template< typename T >
		struct bc_platform_script_ref_pack< core_platform::g_api_win32, T >
		{
			void add_reference()
			{
				_inc_ref(m_value);
			}

			void release_reference()
			{
				_dec_ref(m_value);
			}

			T m_value;
		};

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_ref<TPlatform, T>::bc_platform_script_ref()
		{
		}

		template< core_platform::bc_platform TPlatform, typename T >
		template< typename >
		bc_platform_script_ref< TPlatform, T >::bc_platform_script_ref(type&& p_object)
		{
			m_pack.m_value = std::move(p_object);
			m_pack.add_reference();
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_ref< TPlatform, T >::bc_platform_script_ref(bc_platform_script_ref&&) noexcept = default;

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_ref< TPlatform, T >::~bc_platform_script_ref()
		{
			reset();
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_ref< TPlatform, T >& bc_platform_script_ref< TPlatform, T >::operator=(bc_platform_script_ref&&) noexcept = default;

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_ref<TPlatform, T>& bc_platform_script_ref<TPlatform, T>::operator=(type&& p_object)
		{
			reset(std::move(p_object));

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		typename bc_platform_script_ref< TPlatform, T >::type* bc_platform_script_ref< TPlatform, T >::operator->() noexcept
		{
			return &m_pack.m_value;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		typename bc_platform_script_ref< TPlatform, T >::type& bc_platform_script_ref< TPlatform, T >::operator*()
		{
			return m_pack.m_value;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		typename bc_platform_script_ref<TPlatform, T>::type& bc_platform_script_ref<TPlatform, T>::get()
		{
			return m_pack.m_value;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		const typename bc_platform_script_ref< TPlatform, T >::type& bc_platform_script_ref< TPlatform, T >::get() const
		{
			return m_pack.m_value;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		void bc_platform_script_ref<TPlatform, T>::reset()
		{
			if(m_pack.m_value.is_valid())
			{
				m_pack.release_reference();
				m_pack.m_value = type();
			}
		}

		template< core_platform::bc_platform TPlatform, typename T >
		void bc_platform_script_ref<TPlatform, T>::reset(type&& p_object)
		{
			if (m_pack.m_value.is_valid())
			{
				m_pack.release_reference();
			}
			m_pack.m_value = std::move(p_object);
			m_pack.add_reference();
		}
	}
}
