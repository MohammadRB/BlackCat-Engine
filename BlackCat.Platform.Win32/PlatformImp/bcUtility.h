// [02/04/2016 MRB]

#pragma once

#include "PlatformImp/PlatformImpPCH.h"

namespace black_cat
{
	namespace platform
	{
		BC_PLATFORMIMP_DLL_EXP BOOL win32_from_hresult(HRESULT p_hr, OUT DWORD* p_Win32);

		BC_PLATFORMIMP_DLL_EXP void win_call(bool p_result);

		BC_PLATFORMIMP_DLL_EXP void win_call(HRESULT p_result);
	}
}
