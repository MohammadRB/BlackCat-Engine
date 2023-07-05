// [11/10/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptReference.h"

#include "Platform/Script/bcScriptRef.h"
#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/bcExport.h"

namespace black_cat::platform
{
	template<platform::bc_platform TPlatform>
	class bc_platform_script_context;

	template<platform::bc_platform TPlatform>
	class bc_platform_script_bytecode;

	template<platform::bc_platform TPlatform>
	class bc_platform_script_variable;

	template<platform::bc_platform TPlatform>
	class bc_platform_script_object;

	template<platform::bc_platform TPlatform>
	class bc_platform_script_string;

	template<platform::bc_platform TPlatform>
	class bc_platform_script_function;

	template<platform::bc_platform TPlatform>
	class bc_platform_script_array;

	template<platform::bc_platform TPlatform>
	class bc_platform_script_error;

	template<platform::bc_platform TPlatform, typename T>
	class bc_platform_script_prototype;

	void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_context<platform::g_api_win32>& p_context);

	void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_context<platform::g_api_win32>& p_context);

	void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_bytecode<platform::g_api_win32>& p_byte_code);

	void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_bytecode<platform::g_api_win32>& p_byte_code);

	void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_variable<platform::g_api_win32>& p_variable);

	void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_variable<platform::g_api_win32>& p_variable);

	void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_object<platform::g_api_win32>& p_object);

	void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_object<platform::g_api_win32>& p_object);

	void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_string<platform::g_api_win32>& p_string);

	void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_string<platform::g_api_win32>& p_string);

	void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_function<platform::g_api_win32>& p_function);

	void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_function<platform::g_api_win32>& p_function);

	void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_array<platform::g_api_win32>& p_array);

	void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_array<platform::g_api_win32>& p_array);

	void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_error<platform::g_api_win32>& p_error);

	void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_error<platform::g_api_win32>& p_error);

	template<typename T>
	void BC_PLATFORMIMP_DLL _inc_ref(bc_platform_script_prototype<platform::g_api_win32, T>& p_prototype);

	template<typename T>
	void BC_PLATFORMIMP_DLL _dec_ref(bc_platform_script_prototype<platform::g_api_win32, T>& p_prototype);

	template<typename T>
	struct bc_platform_script_ref_pack<platform::g_api_win32, T>
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

	template<platform::bc_platform TPlatform, typename T>
	bc_platform_script_ref<TPlatform, T>::bc_platform_script_ref()
	{
	}

	template<platform::bc_platform TPlatform, typename T>
	template<typename>
	bc_platform_script_ref<TPlatform, T>::bc_platform_script_ref(const type& p_object)
	{
		reset(p_object);
	}

	template<platform::bc_platform TPlatform, typename T>
	bc_platform_script_ref<TPlatform, T>::bc_platform_script_ref(bc_platform_script_ref&& p_other) noexcept
		: m_pack(std::move(p_other.m_pack))
	{
	}

	template<platform::bc_platform TPlatform, typename T>
	bc_platform_script_ref<TPlatform, T>::~bc_platform_script_ref()
	{
		reset();
	}

	template<platform::bc_platform TPlatform, typename T>
	bc_platform_script_ref<TPlatform, T>& bc_platform_script_ref<TPlatform, T>::operator=(bc_platform_script_ref&& p_other) noexcept
	{
		reset();
		m_pack = std::move(p_other.m_pack);
		p_other.m_pack.m_value = type();

		return *this;
	}

	template<platform::bc_platform TPlatform, typename T>
	bc_platform_script_ref<TPlatform, T>& bc_platform_script_ref<TPlatform, T>::operator=(const type& p_object)
	{
		reset(p_object);

		return *this;
	}

	template<platform::bc_platform TPlatform, typename T>
	typename bc_platform_script_ref<TPlatform, T>::type* bc_platform_script_ref<TPlatform, T>::operator->() noexcept
	{
		return &m_pack.m_value;
	}

	template<platform::bc_platform TPlatform, typename T>
	typename bc_platform_script_ref<TPlatform, T>::type& bc_platform_script_ref<TPlatform, T>::operator*()
	{
		return m_pack.m_value;
	}

	template<platform::bc_platform TPlatform, typename T>
	typename bc_platform_script_ref<TPlatform, T>::type& bc_platform_script_ref<TPlatform, T>::get()
	{
		return m_pack.m_value;
	}

	template<platform::bc_platform TPlatform, typename T>
	const typename bc_platform_script_ref<TPlatform, T>::type& bc_platform_script_ref<TPlatform, T>::get() const
	{
		return m_pack.m_value;
	}

	template<platform::bc_platform TPlatform, typename T>
	void bc_platform_script_ref<TPlatform, T>::reset()
	{
		if(m_pack.m_value.is_valid())
		{
			m_pack.release_reference();
			m_pack.m_value = type();
		}
	}

	template<platform::bc_platform TPlatform, typename T>
	void bc_platform_script_ref<TPlatform, T>::reset(const type& p_object)
	{
		if (m_pack.m_value.is_valid())
		{
			m_pack.release_reference();
		}
		m_pack.m_value = p_object;
		m_pack.add_reference();
	}
}
