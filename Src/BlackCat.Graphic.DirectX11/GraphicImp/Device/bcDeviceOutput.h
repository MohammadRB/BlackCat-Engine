// [13/11/2016 MRB]

#pragma once

#include "Graphic/Device/bcDeviceOutput.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_device_output_pack<g_api_dx11>
		{
			explicit bc_platform_device_output_pack(HWND p_output_handle)
				: m_output_handle(p_output_handle)
			{
			}

			HWND m_output_handle;
		};
	}
}