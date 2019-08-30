// [03/31/2016 MRB]

#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "CorePlatformImp/bcExport.h"
#include "CorePlatformImp/File/bcFileInfo.h"
#include "CorePlatformImp/bcUtility.h"
#include <Shlwapi.h>
#include <Pathcch.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Pathcch.lib")

namespace black_cat
{
	namespace core_platform
	{
		template< >
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_file_info<g_api_win32>::exist(const bcECHAR* p_file_name)
		{
			return PathFileExists(p_file_name);
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_file_info<g_api_win32>::get_basic_info(const bcECHAR* p_file_name, bc_basic_file_info* p_info)
		{
			WIN32_FILE_ATTRIBUTE_DATA l_file_info;

			p_info->m_exist = PathFileExists(p_file_name);
			if(!p_info->m_exist)
			{
				return false;
			}

			const bool l_succeeded = GetFileAttributesEx(p_file_name, GetFileExInfoStandard, &l_file_info);
			if(!l_succeeded)
			{
				return false;
			}

			constexpr bcDOUBLE l_100_nano_to_milli = 1 / 10000.0;
			SYSTEMTIME l_systemtime;
			ULARGE_INTEGER l_ularge_integer;

			FileTimeToSystemTime(&l_file_info.ftCreationTime, &l_systemtime);
			p_info->m_create_time.m_year = l_systemtime.wYear;
			p_info->m_create_time.m_month = l_systemtime.wMonth;
			p_info->m_create_time.m_day_of_week = l_systemtime.wDayOfWeek;
			p_info->m_create_time.m_day = l_systemtime.wDay;
			p_info->m_create_time.m_hour = l_systemtime.wHour;
			p_info->m_create_time.m_minute = l_systemtime.wMinute;
			p_info->m_create_time.m_second = l_systemtime.wSecond;
			p_info->m_create_time.m_milliseconds = l_systemtime.wMilliseconds;
			l_ularge_integer.HighPart = l_file_info.ftCreationTime.dwHighDateTime;
			l_ularge_integer.LowPart = l_file_info.ftCreationTime.dwLowDateTime;
			p_info->m_create_time.m_total_milliseconds = l_ularge_integer.QuadPart * l_100_nano_to_milli;

			FileTimeToSystemTime(&l_file_info.ftLastAccessTime, &l_systemtime);
			p_info->m_last_access_time.m_year = l_systemtime.wYear;
			p_info->m_last_access_time.m_month = l_systemtime.wMonth;
			p_info->m_last_access_time.m_day_of_week = l_systemtime.wDayOfWeek;
			p_info->m_last_access_time.m_day = l_systemtime.wDay;
			p_info->m_last_access_time.m_hour = l_systemtime.wHour;
			p_info->m_last_access_time.m_minute = l_systemtime.wMinute;
			p_info->m_last_access_time.m_second = l_systemtime.wSecond;
			p_info->m_last_access_time.m_milliseconds = l_systemtime.wMilliseconds;
			l_ularge_integer.HighPart = l_file_info.ftLastAccessTime.dwHighDateTime;
			l_ularge_integer.LowPart = l_file_info.ftLastAccessTime.dwLowDateTime;
			p_info->m_last_access_time.m_total_milliseconds = l_ularge_integer.QuadPart * l_100_nano_to_milli;

			FileTimeToSystemTime(&l_file_info.ftLastWriteTime, &l_systemtime);
			p_info->m_last_write_time.m_year = l_systemtime.wYear;
			p_info->m_last_write_time.m_month = l_systemtime.wMonth;
			p_info->m_last_write_time.m_day_of_week = l_systemtime.wDayOfWeek;
			p_info->m_last_write_time.m_day = l_systemtime.wDay;
			p_info->m_last_write_time.m_hour = l_systemtime.wHour;
			p_info->m_last_write_time.m_minute = l_systemtime.wMinute;
			p_info->m_last_write_time.m_second = l_systemtime.wSecond;
			p_info->m_last_write_time.m_milliseconds = l_systemtime.wMilliseconds;
			l_ularge_integer.HighPart = l_file_info.ftLastWriteTime.dwHighDateTime;
			l_ularge_integer.LowPart = l_file_info.ftLastWriteTime.dwLowDateTime;
			p_info->m_last_write_time.m_total_milliseconds = l_ularge_integer.QuadPart * l_100_nano_to_milli;

			l_ularge_integer.u.HighPart = l_file_info.nFileSizeHigh;
			l_ularge_integer.u.LowPart = l_file_info.nFileSizeLow;
			p_info->m_size = l_ularge_integer.QuadPart;

			return true;
		}
	}
}