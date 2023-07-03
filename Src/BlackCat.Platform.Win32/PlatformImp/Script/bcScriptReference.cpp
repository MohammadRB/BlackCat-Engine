// [11/10/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/Script/bcScriptReference.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_reference< platform::g_api_win32 >::bc_platform_script_reference()
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_reference< platform::g_api_win32 >::bc_platform_script_reference(const bc_platform_script_reference& p_other) noexcept
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_reference< platform::g_api_win32 >::~bc_platform_script_reference()
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_reference< platform::g_api_win32 >& bc_platform_script_reference< platform::g_api_win32 >::operator=(const bc_platform_script_reference&) noexcept
		{
			return *this;
		}
	}
}