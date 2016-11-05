// [10/12/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "Platform/bcException.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/Script/bcScriptRuntime.h"
#include "PlatformImp/Script/bcScriptObject.h"
#include "PlatformImp/Script/bcScriptArray.h"
#include "PlatformImp/Script/bcScriptString.h"
#include "PlatformImp/Script/bcScriptError.h"

namespace black_cat
{
	namespace platform
	{
		template< typename T >
		extern void _js_object_finalizer(bc_script_external_object< T >* p_object);

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_object< core_platform::g_api_win32 >::bc_platform_script_object()
		{
			m_pack.m_js_object = JS_INVALID_REFERENCE;
		}

		template<>
		template< typename T >
		BC_PLATFORMIMP_DLL
		bc_platform_script_object< core_platform::g_api_win32 >::bc_platform_script_object(bc_script_context& p_context, bc_script_prototype<T>& p_prototype, T&& p_native_object) noexcept
		{
			bc_chakra_call l_call(p_context);
			bc_script_external_object< T >* l_native_object;
			JsValueRef l_script_object;

			if (!p_prototype.is_valid())
			{
				throw bc_invalid_operation_exception("Invalid prototype object");
			}

			l_native_object = bc_script_runtime::create_native< T, T&& >(std::move(p_native_object));

			l_call = JsCreateExternalObject(l_native_object, reinterpret_cast< JsFinalizeCallback >(&_js_object_finalizer<T>), &l_script_object);
			l_call = JsSetPrototype(l_script_object, p_prototype.get_platform_pack().m_js_prototype);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_object< core_platform::g_api_win32 >::bc_platform_script_object(const bc_platform_script_object& p_other) noexcept
		{
			operator=(p_other);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_object< core_platform::g_api_win32 >::~bc_platform_script_object()
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_object< core_platform::g_api_win32 >& bc_platform_script_object< core_platform::g_api_win32 >::operator=(const bc_platform_script_object& p_other) noexcept
		{
			bc_platform_script_reference::operator=(p_other);
			m_pack.m_js_object = p_other.m_pack.m_js_object;

			return *this;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_script_object< core_platform::g_api_win32 >::has_index(bcUINT32 p_index) const
		{
			bc_chakra_call l_call;
			JsValueRef l_js_index;
			bool l_has_property;

			l_call = JsIntToNumber(p_index, &l_js_index);
			l_call = JsHasIndexedProperty
			(
				m_pack.m_js_object,
				l_js_index,
				&l_has_property
			);

			return l_has_property;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_script_variable bc_platform_script_object< core_platform::g_api_win32 >::get_index(bcUINT32 p_index) const
		{
			bc_chakra_call l_call;
			JsValueRef l_js_index;
			bc_script_variable l_property;

			l_call = JsIntToNumber(p_index, &l_js_index);
			l_call = JsGetIndexedProperty
			(
				m_pack.m_js_object,
				l_js_index,
				&l_property.get_platform_pack().m_js_value
			);

			return l_property;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_script_object< core_platform::g_api_win32 >::has_property(const bcWCHAR* p_property) const 
		{
			bc_chakra_call l_call;
			JsValueRef l_js_property;
			bool l_has_property;

			l_call = JsGetPropertyIdFromName(p_property, &l_js_property);
			l_call = JsHasProperty
			(
				m_pack.m_js_object,
				l_js_property,
				&l_has_property
			);

			return l_has_property;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_script_variable bc_platform_script_object< core_platform::g_api_win32 >::get_property(const bcWCHAR* p_property) const
		{
			bc_chakra_call l_call;
			JsValueRef l_js_property;
			bc_script_variable l_property_value;

			l_call = JsGetPropertyIdFromName(p_property, &l_js_property);
			l_call = JsGetProperty
			(
				m_pack.m_js_object,
				l_js_property,
				&l_property_value.get_platform_pack().m_js_value
			);

			return l_property_value;
		}

		template<>
		BC_PLATFORMIMP_DLL
		core::bc_vector_frame< core::bc_wstring > bc_platform_script_object< core_platform::g_api_win32 >::get_own_property_names() const
		{
			bc_chakra_call l_call;
			JsValueRef l_js_names;

			l_call = JsGetOwnPropertyNames
			(
				m_pack.m_js_object,
				&l_js_names
			);
			l_call.throw_if_faild();

			core::bc_vector_frame< core::bc_wstring > l_property_names;
			bc_script_array<bc_script_string> l_property_name_array;

			static_cast<bc_script_object&>(l_property_name_array).get_platform_pack().m_js_object = l_js_names;

			for(bcUINT32 i = 0, l_end = l_property_name_array.size(); i < l_end; ++i)
			{
				l_property_names.push_back(static_cast< bc_script_string >(l_property_name_array[i]).data());
			}

			return l_property_names;
		}

		template< core_platform::bc_platform TPlatform >
		bool bc_platform_script_object<TPlatform>::is_valid() const noexcept
		{
			return m_pack.m_js_object != JS_INVALID_REFERENCE;
		}
	}
}