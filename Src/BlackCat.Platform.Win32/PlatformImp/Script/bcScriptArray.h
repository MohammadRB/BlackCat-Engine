// [10/12/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptRef.h"
#include "PlatformImp/Script/bcScriptContext.h"
#include "PlatformImp/Script/bcScriptVariable.h"
#include "PlatformImp/Script/bcScriptObject.h"

#include "Platform/Script/bcScriptArray.h"
#include "PlatformImp/Script/bcScriptVariable.h"
#include "PlatformImp/Script/bcScriptError.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_script_array_element_pack<platform::g_api_win32>
		{
			bc_platform_script_array_element_pack(bc_script_array& p_array, bc_script_variable& p_index)
				: m_array(p_array),
				m_index(p_index)
			{
			}

			bc_script_array& m_array;
			bc_script_variable m_index;
		};

		template<>
		struct bc_platform_script_array_pack<platform::g_api_win32>
		{
		};

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_array_wrapper<TPlatform, T>::bc_platform_script_array_wrapper()
			: bc_platform_script_array<TPlatform>()
		{
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_array_wrapper<TPlatform, T>::bc_platform_script_array_wrapper(const bc_platform_script_array<TPlatform>& p_array)
			: bc_platform_script_array<TPlatform>(p_array)
		{
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_array_wrapper<TPlatform, T>::bc_platform_script_array_wrapper(const bc_platform_script_array<TPlatform>& p_array, std::initializer_list<T> p_init)
			: bc_platform_script_array<TPlatform>(p_array)
		{
			bcSIZE l_index = 0;

			for (auto& l_item : p_array)
			{
				(*this)[l_index++] = l_item;
			}
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_array_wrapper<TPlatform, T>::bc_platform_script_array_wrapper(const bc_platform_script_array_wrapper&) noexcept = default;
		
		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_array_wrapper<TPlatform, T>::~bc_platform_script_array_wrapper() = default;

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_array_wrapper<TPlatform, T>& bc_platform_script_array_wrapper<TPlatform, T>::operator=(const bc_platform_script_array_wrapper&) noexcept = default;

		template<platform::bc_platform TPlatform, typename T>
		typename bc_platform_script_array_wrapper<TPlatform, T>::element bc_platform_script_array_wrapper<TPlatform, T>::operator[](bcINT p_index)
		{
			return element(*this, p_index);
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_array_wrapper_element<TPlatform, T>::bc_platform_script_array_wrapper_element(bc_platform_script_array_element<TPlatform>& p_element)
			: bc_platform_script_array_element<TPlatform>(p_element)
		{
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_array_wrapper_element<TPlatform, T>& bc_platform_script_array_wrapper_element<TPlatform, T>::operator=(T p_value)
		{
			bc_platform_script_array_element<TPlatform>::operator=(bc_script_variable::_pack_arg(p_value));
			return *this;
		}

		template<platform::bc_platform TPlatform, typename T>
		bc_platform_script_array_wrapper_element<TPlatform, T>::operator T()
		{
			T l_result;
			bc_script_variable::_unpack_arg(static_cast<bc_script_variable>(*this), &l_result);

			return l_result;
		}
	}
}