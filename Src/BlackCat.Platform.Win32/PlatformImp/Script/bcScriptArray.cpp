// [11/10/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/Script/bcScriptArray.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_array_base< core_platform::g_api_win32 >::bc_platform_script_array_base()
			: bc_platform_script_object()
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_array_base< core_platform::g_api_win32 >::bc_platform_script_array_base(bc_script_context& p_context, bcSIZE p_length)
		{
			bc_chakra_call l_call(p_context);

			l_call = JsCreateArray(p_length, &bc_platform_script_object::get_platform_pack().m_js_object);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_array_base< core_platform::g_api_win32 >::bc_platform_script_array_base(bc_script_context& p_context, std::initializer_list< bc_script_variable > p_array)
			: bc_platform_script_array_base(p_context, p_array.size())
		{
			bcSIZE l_index = 0;

			for (auto& l_item : p_array)
			{
				(*this)[l_index++] = l_item;
			}
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_array_base< core_platform::g_api_win32 >::bc_platform_script_array_base(const bc_platform_script_array_base& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_array_base< core_platform::g_api_win32 >::~bc_platform_script_array_base()
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_array_base< core_platform::g_api_win32 >& bc_platform_script_array_base< core_platform::g_api_win32 >::operator=(const bc_platform_script_array_base& p_other) noexcept
		{
			bc_platform_script_object::operator=(std::move(p_other));

			return *this;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bcSIZE bc_platform_script_array_base< core_platform::g_api_win32 >::size() const noexcept
		{
			auto l_length = bc_platform_script_object::get_property(L"length").as_integer();

			return l_length;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_array_base< core_platform::g_api_win32 >::element bc_platform_script_array_base< core_platform::g_api_win32 >::operator[](bcINT p_index)
		{
			return element(*this, p_index);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_array_base_element< core_platform::g_api_win32 >::bc_platform_script_array_base_element(bc_platform_script_array_base< core_platform::g_api_win32 >& p_array, bcINT p_index)
			: m_pack(p_array, bc_script_variable::_pack_arg(p_index))
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_array_base_element< core_platform::g_api_win32 >& bc_platform_script_array_base_element< core_platform::g_api_win32 >::operator=(bc_script_variable p_value)
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

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_array_base_element< core_platform::g_api_win32 >::operator bc_script_variable()
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
	}
}
