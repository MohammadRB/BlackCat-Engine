 // [10/24/2016 MRB]

#pragma once

#include "PlatformImp/Application/bcWindow.h"

#include "Platform/Application/bcConsoleWindow.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_console_window_pack< core_platform::g_api_win32 >
		{
			core::bc_estring m_caption;
			HWND m_hwnd;
			HANDLE m_std_out;
		};
	}
}