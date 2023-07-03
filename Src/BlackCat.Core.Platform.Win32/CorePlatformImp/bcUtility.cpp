// [04/02/2016 MRB]

#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "CorePlatformImp/bcUtility.h"
#include "CorePlatform/bcException.h"

namespace black_cat
{
	namespace platform
	{
		// https://blogs.msdn.microsoft.com/oldnewthing/20061103-07/?p=29133
		BOOL win32_from_hresult(HRESULT p_hr, OUT DWORD *p_Win32)
		{
			if ((p_hr & 0xFFFF0000) == MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0)) 
			{
				// Could have come from many values, but we choose this one
				*p_Win32 = HRESULT_CODE(p_hr);
				return TRUE;
			}
			if (p_hr == S_OK) 
			{
				*p_Win32 = HRESULT_CODE(p_hr);
				return TRUE;
			}
			// otherwise, we got an impossible value
			return FALSE;
		}

		void win_call(DWORD p_error_code)
		{
			bcCHAR l_error_string[MAX_PATH];

			FormatMessageA
			(
				FORMAT_MESSAGE_FROM_SYSTEM,
				0,
				p_error_code,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				l_error_string,
				MAX_PATH,
				NULL
			);

			std::string l_message = "Win32 Error: ";
			l_message += l_error_string;

			throw bc_platform_exception(static_cast<bcINT>(p_error_code), l_message);
		}

		void win_call(bool p_result)
		{
			if(!p_result)
			{
				const DWORD l_error_code = GetLastError();
				win_call(l_error_code);
			}
		}

		void win_call(HRESULT p_result)
		{
			if(p_result != S_OK)
			{
				DWORD l_error_code = 0;
				win32_from_hresult(p_result, &l_error_code);
				
				if(l_error_code != 0)
				{
					win_call(l_error_code);
				}
			}
		}

		DWORD bc_platform_cast(bc_file_access p_file_access)
		{
			DWORD l_result = 0;

			switch (p_file_access)
			{
			case bc_file_access::read:
				l_result = GENERIC_READ;
				break;
			case bc_file_access::write:
				l_result = GENERIC_WRITE;
				break;
			case bc_file_access::read_write:
				l_result = GENERIC_READ | GENERIC_WRITE;
			}

			return l_result;
		}

		DWORD bc_platform_cast(bc_file_sharing p_file_access)
		{
			DWORD l_result = 0;

			switch (p_file_access)
			{
			case bc_file_sharing::none:
				l_result = 0;
				break;
			case bc_file_sharing::read:
				l_result = FILE_SHARE_READ;
				break;
			case bc_file_sharing::write:
				l_result = FILE_SHARE_WRITE | FILE_SHARE_DELETE;
				break;
			case bc_file_sharing::read_write:
				l_result = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
				break;
			}

			return l_result;
		}

		DWORD bc_platform_cast(bc_file_mode p_file_mode)
		{
			DWORD l_result = 0;

			switch (p_file_mode)
			{
			case bc_file_mode::create:
				l_result = CREATE_NEW;
				break;
			case bc_file_mode::create_overwrite:
				l_result = CREATE_ALWAYS;
				break;
			case bc_file_mode::create_append:
				l_result = OPEN_ALWAYS;
				break;
			case bc_file_mode::open:
				l_result = OPEN_EXISTING;
				break;
			case bc_file_mode::open_create:
				l_result = OPEN_ALWAYS;
				break;
			case bc_file_mode::open_truncate:
				l_result = TRUNCATE_EXISTING;
				break;
			}

			return l_result;
		}

		DWORD bc_platform_cast(bc_file_seek p_file_seek)
		{
			DWORD l_result = 0;

			switch (p_file_seek)
			{
			case bc_file_seek::start:
				l_result = FILE_BEGIN;
				break;
			case bc_file_seek::current:
				l_result = FILE_CURRENT;
				break;
			case bc_file_seek::end:
				l_result = FILE_END;
				break;
			}

			return l_result;
		}
	}
}