// [18/06/2021 MRB]

#pragma once

#include "GraphicImp/bcDeviceRef.h"
#include "GraphicImp/bcDeviceReference.h"

#include "Graphic/Device/bcDeviceSwapBuffer.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_device_swap_buffer_pack<g_api_dx11> : public bc_platform_device_reference_pack<g_api_dx11>
		{
			bool m_vsync;
			IDXGISwapChain* m_swap_chain;
		};
	}	
}