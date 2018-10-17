// [02/02/2016 MRB]

#pragma once

#include "CorePlatform/File/bcPath.h"

namespace black_cat
{
	namespace core_platform
	{
		template< >
		struct bc_platform_path_pack< g_api_win32 >
		{
		public:
#ifdef BC_UNICODE
			using path_string = std::wstring;
#else
			using path_string = std::string;
#endif

			bc_platform_path_pack(const path_string& p_path)
				: m_path(p_path)
			{
			}

			bc_platform_path_pack(path_string&& p_path)
				: m_path(std::move(p_path))
			{
			}

			path_string m_path;
		};
	}
}