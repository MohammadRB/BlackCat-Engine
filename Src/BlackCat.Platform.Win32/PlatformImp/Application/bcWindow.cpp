// [10/23/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/Application/bcWindow.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_window<g_api_win32>::bc_platform_window()
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_window<g_api_win32>::bc_platform_window(bc_platform_window&& p_other) noexcept
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_window<g_api_win32>::~bc_platform_window()
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_window<g_api_win32>& bc_platform_window<g_api_win32>::operator=(bc_platform_window&& p_other) noexcept
		{
			return *this;
		}
	}
}
