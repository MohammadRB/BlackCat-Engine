// [10/11/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/Script/bcScriptVariable.h"
#include "PlatformImp/Script/bcScriptString.h"
#include "PlatformImp/Script/bcScriptObject.h"
#include "PlatformImp/Script/bcScriptArray.h"
#include "PlatformImp/Script/bcScriptFunction.h"
#include "PlatformImp/Script/bcScriptError.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_variable< core_platform::g_api_win32 >::bc_platform_script_variable() noexcept
		{
			m_pack.m_js_value = JS_INVALID_REFERENCE;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_variable< core_platform::g_api_win32 >::bc_platform_script_variable(bc_script_context& p_context, bool p_bool)
		{
			as_boolean(p_bool);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_variable< core_platform::g_api_win32 >::bc_platform_script_variable(bc_script_context& p_context, bcINT p_int)
		{
			as_integer(p_int);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_variable< core_platform::g_api_win32 >::bc_platform_script_variable(bc_script_context& p_context, bcDOUBLE p_double)
		{
			as_double(p_double);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_variable< core_platform::g_api_win32 >::bc_platform_script_variable(bc_script_context& p_context, bc_script_string& p_string)
		{
			as_string(p_string);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_variable< core_platform::g_api_win32 >::bc_platform_script_variable(bc_script_context& p_context, bc_script_object& p_object)
		{
			as_object(p_object);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_variable< core_platform::g_api_win32 >::bc_platform_script_variable(bc_script_context& p_context, bc_script_error& p_error)
		{
			as_error(p_error);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_variable< core_platform::g_api_win32 >::bc_platform_script_variable(const bc_platform_script_variable& p_other) noexcept
		{
			operator=(p_other);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_variable< core_platform::g_api_win32 >::~bc_platform_script_variable()
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_variable< core_platform::g_api_win32 >& bc_platform_script_variable< core_platform::g_api_win32 >::operator=(const bc_platform_script_variable& p_other) noexcept
		{
			bc_platform_script_reference::operator=(p_other);
			m_pack.m_js_value = p_other.m_pack.m_js_value;

			return *this;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_script_value_type bc_platform_script_variable< core_platform::g_api_win32 >::get_type() const noexcept
		{
			bc_script_value_type l_type;
			JsValueType l_js_type;

			JsGetValueType(m_pack.m_js_value, &l_js_type);

			switch (l_js_type)
			{
			case JsBoolean:
				l_type = bc_script_value_type::boolean;
				break;
			case JsNumber:
				l_type = bc_script_value_type::number;
				break;
			case JsString:
				l_type = bc_script_value_type::string;
				break;
			case JsNull:
				l_type = bc_script_value_type::null;
				break;
			case JsUndefined:
				l_type = bc_script_value_type::undefined;
				break;
			case JsObject:
				l_type = bc_script_value_type::object;
				break;
			case JsFunction:
				l_type = bc_script_value_type::function;
				break;
			case JsArray:
				l_type = bc_script_value_type::array;
				break;
			case JsError:
			default:
				l_type = bc_script_value_type::object;
				break;
			}

			return l_type;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_script_variable< core_platform::g_api_win32 >::is_boolean() const noexcept
		{
			JsValueType l_js_type;

			JsGetValueType(m_pack.m_js_value, &l_js_type);

			return l_js_type == JsBoolean;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_script_variable< core_platform::g_api_win32 >::is_number() const noexcept
		{
			JsValueType l_js_type;

			JsGetValueType(m_pack.m_js_value, &l_js_type);

			return l_js_type == JsNumber;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_script_variable< core_platform::g_api_win32 >::is_string() const noexcept
		{
			JsValueType l_js_type;

			JsGetValueType(m_pack.m_js_value, &l_js_type);

			return l_js_type == JsString;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_script_variable< core_platform::g_api_win32 >::is_object() const noexcept
		{
			JsValueType l_js_type;

			JsGetValueType(m_pack.m_js_value, &l_js_type);

			return l_js_type == JsObject;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_script_variable< core_platform::g_api_win32 >::is_function() const noexcept
		{
			JsValueType l_js_type;

			JsGetValueType(m_pack.m_js_value, &l_js_type);

			return l_js_type == JsFunction;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_script_variable< core_platform::g_api_win32 >::is_array() const noexcept
		{
			JsValueType l_js_type;

			JsGetValueType(m_pack.m_js_value, &l_js_type);

			return l_js_type == JsArray;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_script_variable< core_platform::g_api_win32 >::is_error() const noexcept
		{
			JsValueType l_js_type;

			JsGetValueType(m_pack.m_js_value, &l_js_type);

			return l_js_type == JsError;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_script_variable< core_platform::g_api_win32 >::is_null() const noexcept
		{
			JsValueType l_js_type;

			JsGetValueType(m_pack.m_js_value, &l_js_type);

			return l_js_type == JsNull;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_script_variable< core_platform::g_api_win32 >::is_undefined() const noexcept
		{
			JsValueType l_js_type;

			JsGetValueType(m_pack.m_js_value, &l_js_type);

			return l_js_type == JsUndefined;
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_script_variable< core_platform::g_api_win32 >::as_boolean(bool p_bool)
		{
			//JsConvertValueToBoolean(m_pack.m_js_value, &m_pack.m_js_value);
			JsBoolToBoolean(p_bool, &m_pack.m_js_value);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_script_variable< core_platform::g_api_win32 >::as_boolean() const
		{
			bcAssert(is_boolean());

			bool l_value;

			JsBooleanToBool(m_pack.m_js_value, &l_value);

			return l_value;
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_script_variable< core_platform::g_api_win32 >::as_integer(bcINT p_int)
		{
			//JsConvertValueToNumber(m_pack.m_js_value, &m_pack.m_js_value);
			JsIntToNumber(p_int, &m_pack.m_js_value);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bcINT bc_platform_script_variable< core_platform::g_api_win32 >::as_integer() const
		{
			bcAssert(is_number());

			bcINT l_value;

			JsNumberToInt(m_pack.m_js_value, &l_value);

			return l_value;
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_script_variable< core_platform::g_api_win32 >::as_double(bcDOUBLE p_value)
		{
			//JsConvertValueToNumber(m_pack.m_js_value, &m_pack.m_js_value);
			JsDoubleToNumber(p_value, &m_pack.m_js_value);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bcDOUBLE bc_platform_script_variable< core_platform::g_api_win32 >::as_double() const
		{
			bcAssert(is_number());

			bcDOUBLE l_value;

			JsNumberToDouble(m_pack.m_js_value, &l_value);

			return l_value;
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_script_variable< core_platform::g_api_win32 >::as_string(bc_script_string& p_string)
		{
			m_pack.m_js_value = p_string.get_platform_pack().m_js_string;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_script_string bc_platform_script_variable< core_platform::g_api_win32 >::as_string() const
		{
			bcAssert(is_string());

			bc_script_string l_string;

			l_string.get_platform_pack().m_js_string = m_pack.m_js_value;

			return l_string;
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_script_variable< core_platform::g_api_win32 >::as_object(bc_script_object& p_object)
		{
			m_pack.m_js_value = p_object.get_platform_pack().m_js_object;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_script_object bc_platform_script_variable< core_platform::g_api_win32 >::as_object() const
		{
			bcAssert(is_object());

			bc_script_object l_object;

			l_object.get_platform_pack().m_js_object = m_pack.m_js_value;

			return l_object;
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_script_variable< core_platform::g_api_win32 >::as_error(bc_script_error& p_error)
		{
			m_pack.m_js_value = p_error.get_platform_pack().m_js_error;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_script_error bc_platform_script_variable< core_platform::g_api_win32 >::as_error() const
		{
			bcAssert(is_error());

			bc_script_error l_error;

			l_error.get_platform_pack().m_js_error = m_pack.m_js_value;

			return l_error;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_script_variable< core_platform::g_api_win32 >::equals(const bc_script_variable& p_other) const noexcept
		{
			bc_chakra_call l_call;
			bool l_result;
			
			l_call = JsEquals(m_pack.m_js_value, const_cast<bc_script_variable&>(p_other).get_platform_pack().m_js_value, &l_result);

			return l_result;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_script_variable< core_platform::g_api_win32 >::strict_equals(const bc_script_variable& p_other) const noexcept
		{
			bc_chakra_call l_call;
			bool l_result;

			l_call = JsEquals(m_pack.m_js_value, const_cast<bc_script_variable&>(p_other).get_platform_pack().m_js_value, &l_result);

			return l_result;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_script_variable< core_platform::g_api_win32 >::is_valid() const noexcept
		{
			return m_pack.m_js_value != JS_INVALID_REFERENCE;
		}
	}
}
