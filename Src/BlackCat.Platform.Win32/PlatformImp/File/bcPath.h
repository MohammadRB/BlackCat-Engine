// [02/02/2016 MRB]

#pragma once

#include "Platform/File/bcPath.h"

namespace black_cat
{
	namespace platform
	{
		template< >
		struct bc_platform_path_pack< core_platform::g_api_win32 >
			: public bc_platform_path_pack< core_platform::g_api_unknown >
		{
			bc_platform_path_pack(core::bc_estring&& p_path) // See definition of bc_platform_path construction for usage of r-value parameter
				: m_path(move(p_path))
			{
			}

			core::bc_estring m_path;
		};
	}
}
