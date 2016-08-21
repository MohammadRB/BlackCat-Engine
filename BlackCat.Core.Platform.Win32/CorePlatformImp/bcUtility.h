// [02/04/2016 MRB]

#pragma once

#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "CorePlatformImp/bcExport.h"
#include "CorePlatform/File/bcFile.h"

namespace black_cat
{
	namespace core_platform
	{
		BC_COREPLATFORMIMP_DLL BOOL win32_from_hresult(HRESULT p_hr, OUT DWORD* p_Win32);

		BC_COREPLATFORMIMP_DLL void win_call(DWORD p_result);

		BC_COREPLATFORMIMP_DLL void win_call(bool p_result);

		BC_COREPLATFORMIMP_DLL void win_call(HRESULT p_result);

		DWORD bc_platform_cast(bc_file_access p_file_access);

		DWORD bc_platform_cast(bc_file_sharing p_file_access);

		DWORD bc_platform_cast(bc_file_mode p_file_mode);

		DWORD bc_platform_cast(bc_file_seek p_file_seek);
	}
}
