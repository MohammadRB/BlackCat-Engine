// [12/22/2016 MRB]

#pragma once

#include "Graphic/bcDeviceReference.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_device_reference_pack<g_api_dx11>
		{
		};
	}
}