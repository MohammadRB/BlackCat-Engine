// [04/15/2016 MRB]

#pragma once

#include "Platform/Application/bcKeyDevice.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_key_device_pack<core_platform::g_api_win32>
		{
			bc_key_state m_state[256];
		};
	}
}