// [03/26/2016 MRB]

#pragma once

#include "CorePlatform/File/bcFile.h"
#include "CorePlatformImp/CorePlatformImpPCH.h"

namespace black_cat
{
	namespace core_platform
	{
		template< >
		struct bc_platform_file_pack< core_platform::g_api_win32 > 
			: public bc_platform_file_pack< core_platform::g_api_unknown >
		{
			HANDLE m_file;
			DWORD m_error_code;

		};
	}
}