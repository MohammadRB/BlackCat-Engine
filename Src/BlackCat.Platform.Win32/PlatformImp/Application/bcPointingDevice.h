// [15/04/2016 MRB]

#pragma once

#include "Platform/Application/bcPointingDevice.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_pointing_device_pack<platform::g_api_win32>
		{
			bc_pointing_device_state m_state;
		};
	}
}