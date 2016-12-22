// [10/11/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptReference.h"

#include "Platform/Script/bcScriptRef.h"
#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/bcExport.h"

namespace black_cat
{
	namespace platform
	{
		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_context;

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_bytecode;

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_variable;

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_object;

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_string;

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_function_base;

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_array_base;

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_error;

		template< core_platform::bc_platform TPlatform, typename T >
		class bc_platform_script_prototype;

		void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_context<core_platform::g_api_win32>& p_context);

		void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_context<core_platform::g_api_win32>& p_context);

		void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_bytecode<core_platform::g_api_win32>& p_byte_code);

		void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_bytecode<core_platform::g_api_win32>& p_byte_code);

		void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_variable<core_platform::g_api_win32>& p_variable);

		void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_variable<core_platform::g_api_win32>& p_variable);

		void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_object<core_platform::g_api_win32>& p_object);

		void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_object<core_platform::g_api_win32>& p_object);

		void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_string<core_platform::g_api_win32>& p_string);

		void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_string<core_platform::g_api_win32>& p_string);

		void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_function_base<core_platform::g_api_win32>& p_function);

		void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_function_base<core_platform::g_api_win32>& p_function);

		void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_array_base<core_platform::g_api_win32>& p_array);

		void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_array_base<core_platform::g_api_win32>& p_array);

		void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_error<core_platform::g_api_win32>& p_error);

		void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_error<core_platform::g_api_win32>& p_error);

		template< typename T >
		void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_prototype<core_platform::g_api_win32, T>& p_error);

		template< typename T >
		void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_prototype<core_platform::g_api_win32, T>& p_error);

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
		bc_platform_script_ref< TPlatform, T >::bc_platform_script_ref(type& p_object)
		{
			reset(p_object);
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
		bc_platform_script_ref<TPlatform, T>& bc_platform_script_ref<TPlatform, T>::operator=(type& p_object)
		{
			reset(p_object);

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
		void bc_platform_script_ref<TPlatform, T>::reset(type& p_object)
		{
			if (m_pack.m_value.is_valid())
			{
				m_pack.release_reference();
			}
			m_pack.m_value = p_object;
			m_pack.add_reference();
		}
	}
}