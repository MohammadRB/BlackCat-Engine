 // [11/10/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/Script/bcScriptError.h"

namespace black_cat::platform
{
	template<>
	BC_PLATFORMIMP_DLL
	bc_platform_script_error<platform::g_api_win32>::bc_platform_script_error(bc_script_context& p_context, const core::bc_wstring& p_message)
	{
		bc_chakra_call l_call(p_context);
		JsValueRef l_error_value;

		l_call = JsPointerToString(p_message.c_str(), p_message.length(), &l_error_value);
		l_call = JsCreateError(l_error_value, &m_pack.m_js_error);
	}

	template<>
	BC_PLATFORMIMP_DLL
	bc_platform_script_error<platform::g_api_win32>::bc_platform_script_error()
	{
		m_pack.m_js_error = JS_INVALID_REFERENCE;
	}

	template<>
	BC_PLATFORMIMP_DLL
	bc_platform_script_error<platform::g_api_win32>::bc_platform_script_error(const bc_platform_script_error& p_other) noexcept
	{
		operator=(p_other);
	}

	template<>
	BC_PLATFORMIMP_DLL
	bc_platform_script_error<platform::g_api_win32>::~bc_platform_script_error()
	{	
	}

	template<>
	BC_PLATFORMIMP_DLL
	bc_platform_script_error<platform::g_api_win32>& bc_platform_script_error<platform::g_api_win32>::operator=(const bc_platform_script_error& p_other) noexcept
	{
		bc_platform_script_reference::operator=(p_other);
		m_pack.m_js_error = p_other.m_pack.m_js_error;

		return *this;
	}

	template<>
	BC_PLATFORMIMP_DLL
	core::bc_wstring bc_platform_script_error<platform::g_api_win32>::error_message() const
	{
		bc_chakra_call l_call;
		JsValueRef l_message_property;
		JsPropertyIdRef l_message_id;
		JsValueType l_message_type;

		l_call = JsGetPropertyIdFromName(L"message", &l_message_id);
		l_call = JsGetProperty(m_pack.m_js_error, l_message_id, &l_message_property);

		if(!l_call.successed())
		{
			l_call.clear();
			return L"";
		}

		JsGetValueType(l_message_property, &l_message_type);

		if(l_message_type != JsString)
		{
			return L"";
		}

		bcSIZE l_str_lenght;
		const bcWCHAR* l_str_data;

		l_call = JsStringToPointer(l_message_property, &l_str_data, &l_str_lenght);

		if(!l_call.successed())
		{
			l_call.clear();
			return L"";
		}

		return core::bc_wstring(l_str_data);
	}

	template<>
	BC_PLATFORMIMP_DLL
	bool bc_platform_script_error<platform::g_api_win32>::is_valid() const noexcept
	{
		return m_pack.m_js_error != JS_INVALID_REFERENCE;
	}
}
