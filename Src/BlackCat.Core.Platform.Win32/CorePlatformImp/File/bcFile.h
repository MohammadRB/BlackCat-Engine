// [03/26/2016 MRB]

#pragma once

#include "CorePlatform/File/bcFile.h"
#include "CorePlatformImp/CorePlatformImpPCH.h"

namespace black_cat
{
	namespace core_platform
	{
		template< >
		struct bc_platform_file_pack< g_api_win32 > 
		{
			HANDLE m_file;
			DWORD m_error_code;
		};
	}
}