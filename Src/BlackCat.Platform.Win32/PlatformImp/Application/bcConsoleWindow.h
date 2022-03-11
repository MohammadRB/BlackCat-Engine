 // [10/24/2016 MRB]

#pragma once

#include "PlatformImp/Application/bcWindow.h"

#include "Platform/Application/bcConsoleWindow.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_console_window_pack<platform::g_api_win32>
		{
			HWND m_handle;
			HANDLE m_std_out;
		};
	}
}