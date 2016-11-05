// [10/12/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptContext.h"
#include "PlatformImp/Script/bcScriptObject.h"

#include "Platform/Script/bcScriptArray.h"
#include "PlatformImp/Script/bcScriptVariable.h"
#include "PlatformImp/Script/bcScriptError.h"

namespace black_cat
{
	namespace platform
	{
		template< typename T >
		struct bc_platform_script_array_pack< core_platform::g_api_win32, T >
		{
		};

		template< typename T >
		struct bc_platform_script_array_element_pack< core_platform::g_api_win32, T >
		{
			bc_platform_script_array_element_pack(bc_script_array< T >& p_array, bc_script_variable p_index)
				: m_array(p_array),
				m_index(p_index)
			{
			}

			bc_script_array< T >& m_array;
			bc_script_variable m_index;
		};

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array< TPlatform, T >::bc_platform_script_array() = default;

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array< TPlatform, T >::bc_platform_script_array(bc_script_context& p_context, bcSIZE p_length)
		{
			bc_chakra_call l_call(p_context);

			l_call = JsCreateArray(p_length, &static_cast< bc_script_variable& >(this).get_platform_pack().m_js_value);
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array< TPlatform, T >::bc_platform_script_array(bc_script_context& p_context, std::initializer_list< T > p_array)
			: bc_platform_script_array(p_array.size())
		{
			bcSIZE l_index = 0;

			for (auto& l_item : p_array)
			{
				(*this)[l_index++] = l_item;
			}
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array< TPlatform, T >::bc_platform_script_array(const bc_platform_script_array&) noexcept = default;

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array< TPlatform, T >::~bc_platform_script_array() = default;

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array< TPlatform, T >& bc_platform_script_array< TPlatform, T >::operator=(const bc_platform_script_array&) noexcept = default;

		template< core_platform::bc_platform TPlatform, typename T >
		bcSIZE bc_platform_script_array< TPlatform, T >::size() const noexcept
		{
			auto l_length = bc_platform_script_object< TPlatform >::get_property(L"length").as_integer();

			return l_length;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		typename bc_platform_script_array< TPlatform, T >::element bc_platform_script_array< TPlatform, T >::operator[](bcINT p_index)
		{
			return element(*this, p_index);
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array_element< TPlatform, T >::bc_platform_script_array_element(bc_script_array< T >& p_array, bcINT p_index)
			: m_pack(p_array, bc_script_variable::_pack_arg(p_index))
		{
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array_element< TPlatform, T >& bc_platform_script_array_element< TPlatform, T >::operator=(bc_script_variable p_value)
		{
			bc_chakra_call l_call;

			l_call = JsSetIndexedProperty
			(
				static_cast< bc_script_object& >(m_pack.m_array).get_platform_pack().m_js_object,
				m_pack.m_index.get_platform_pack().m_js_value,
				p_value.get_platform_pack().m_js_value
			);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array_element<TPlatform, T>& bc_platform_script_array_element<TPlatform, T>::operator=(T p_value)
		{
			(*this) = bc_script_variable::_pack_arg(p_value);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array_element< TPlatform, T >::operator bc_script_variable()
		{
			bc_chakra_call l_call;
			bc_script_variable l_value;

			l_call = JsGetIndexedProperty
			(
				static_cast< bc_script_object& >(m_pack.m_array).get_platform_pack().m_js_object,
				m_pack.m_index.get_platform_pack().m_js_value,
				&l_value.get_platform_pack().m_js_value
			);

			return l_value;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array_element<TPlatform, T>::operator T()
		{
			T l_result;
			bc_script_variable::_unpack_arg(static_cast< bc_script_variable >(*this), &l_result);

			return l_result;
		}
	}
}
