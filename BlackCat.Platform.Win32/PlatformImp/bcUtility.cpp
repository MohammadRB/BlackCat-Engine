// [02/04/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/bcUtility.h"

namespace black_cat
{
	namespace platform
	{
		// https://blogs.msdn.microsoft.com/oldnewthing/20061103-07/?p=29133
		BOOL win32_from_hresult(HRESULT p_hr, OUT DWORD *p_Win32)
		{
			if ((p_hr & 0xFFFF0000) == MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0)) {
				// Could have come from many values, but we choose this one
				*p_Win32 = HRESULT_CODE(p_hr);
				return TRUE;
			}
			if (p_hr == S_OK) {
				*p_Win32 = HRESULT_CODE(p_hr);
				return TRUE;
			}
			// otherwise, we got an impossible value
			return FALSE;
		}

		void win_call(bool p_result)
		{
			if(!p_result)
			{
				DWORD l_error_code = GetLastError();
				bcCHAR l_error_string[MAX_PATH];

				FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
					0,
					l_error_code,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					l_error_string,
					MAX_PATH,
					NULL);

				core::bc_string l_message = "Win32 Error: ";
				l_message += l_error_string;

				throw bc_platform_exception(static_cast<bcINT>(l_error_code), l_message);
			}
		};

		void win_call(HRESULT p_result)
		{
			if (!p_result)
			{
				DWORD l_error_code = 0;
				win32_from_hresult(p_result, &l_error_code);
				bcCHAR l_error_string[MAX_PATH];

				FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
					0,
					l_error_code,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					l_error_string,
					MAX_PATH,
					NULL);

				core::bc_string l_message = "Win32 Error: ";
				l_message += l_error_string;

				throw bc_platform_exception(static_cast<bcINT>(l_error_code), l_message);
			}
		}
	}
}