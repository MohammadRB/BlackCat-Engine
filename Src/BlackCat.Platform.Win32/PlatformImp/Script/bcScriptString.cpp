 // [12/10/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/Script/bcScriptString.h"

namespace black_cat::platform
{
	template<>
	BC_PLATFORMIMP_DLL
	bc_platform_script_string<platform::g_api_win32>::bc_platform_script_string()
		: bc_platform_script_reference(),
		  m_pack()
	{
	}

	template<>
	BC_PLATFORMIMP_DLL
	bc_platform_script_string<platform::g_api_win32>::bc_platform_script_string(bc_script_context& p_context, const core::bc_wstring& p_value)
		: bc_platform_script_reference(),
		  m_pack()
	{
		JsPointerToString(p_value.data(), p_value.size(), &m_pack.m_js_string);
	}

	template<>
	BC_PLATFORMIMP_DLL
	bc_platform_script_string<platform::g_api_win32>::bc_platform_script_string(const bc_platform_script_string& p_other) noexcept
		: bc_platform_script_reference(p_other),
		  m_pack(p_other.m_pack)
	{
	}

	template<>
	BC_PLATFORMIMP_DLL
	bc_platform_script_string<platform::g_api_win32>::~bc_platform_script_string() = default;

	template<>
	BC_PLATFORMIMP_DLL
	bc_platform_script_string<platform::g_api_win32>& bc_platform_script_string<platform::g_api_win32>::operator=(const bc_platform_script_string& p_other) noexcept
	{
		bc_platform_script_reference::operator=(p_other);
		m_pack.m_js_string = p_other.m_pack.m_js_string;

		return *this;
	}

	template<>
	BC_PLATFORMIMP_DLL
	bcSIZE bc_platform_script_string<platform::g_api_win32>::length() const noexcept
	{
		bcINT l_length;

		JsGetStringLength(m_pack.m_js_string, &l_length);

		return l_length;
	}

	template<>
	BC_PLATFORMIMP_DLL
	core::bc_wstring bc_platform_script_string<platform::g_api_win32>::data() const
	{
		bcSIZE l_length;
		const bcWCHAR* l_string;

		JsStringToPointer
		(
			m_pack.m_js_string,
			&l_string,
			&l_length
		);

		return core::bc_wstring(l_string);
	}

	template<>
	BC_PLATFORMIMP_DLL
	bool bc_platform_script_string<platform::g_api_win32>::is_valid() const noexcept
	{
		return m_pack.m_js_string != JS_INVALID_REFERENCE;
	}
}
