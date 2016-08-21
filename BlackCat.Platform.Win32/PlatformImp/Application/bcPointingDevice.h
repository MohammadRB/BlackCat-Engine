// [04/15/2016 MRB]

#pragma once

#include "Platform/Application/bcPointingDevice.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_pointing_device_pack<core_platform::g_api_win32>
		{
			bc_pointing_device_state m_state;
		};
	}
}