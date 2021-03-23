// [10/11/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/Script/bcScriptByteCode.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_bytecode< core_platform::g_api_win32 >::bc_platform_script_bytecode()
			: bc_platform_script_reference()
		{
			m_pack.m_js_parsed_script = JS_INVALID_REFERENCE;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_bytecode< core_platform::g_api_win32 >::bc_platform_script_bytecode(const bc_platform_script_bytecode& p_other) noexcept
		{
			operator=(p_other);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_bytecode< core_platform::g_api_win32 >::~bc_platform_script_bytecode()
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_bytecode< core_platform::g_api_win32 >& bc_platform_script_bytecode< core_platform::g_api_win32 >::operator=(const bc_platform_script_bytecode& p_other) noexcept
		{
			bc_platform_script_reference::operator=(p_other);
			m_pack = p_other.m_pack;

			return *this;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_script_bytecode< core_platform::g_api_win32 >::is_valid() const noexcept
		{
			return m_pack.m_js_parsed_script != JS_INVALID_REFERENCE;
		}
	}
}