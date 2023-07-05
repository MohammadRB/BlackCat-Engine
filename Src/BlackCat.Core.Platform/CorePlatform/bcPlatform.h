// [08/10/2014 MRB]

#pragma once

namespace black_cat::platform
{
	enum class bc_platform
	{
		unknown = 0,
		win32 = 1
	};

	constexpr bc_platform g_api_unknown = bc_platform::unknown;
	constexpr bc_platform g_api_win32 = bc_platform::win32;
	constexpr bc_platform g_current_platform = g_api_win32;
}
