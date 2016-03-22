// [01/23/2016 MRB]

#pragma once

#include "GraphicImp/Resource/bcResource.h"

#include "Graphic/Device/bcDevice.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_device_pack<bc_platform_render_api::directx11>
			: public bc_platform_device_pack<bc_platform_render_api::unknown>
		{
			bool m_vsync;
			Microsoft::WRL::ComPtr< ID3D11Device > m_device;
			Microsoft::WRL::ComPtr< ID3D11DeviceContext > m_immediate_context;
			Microsoft::WRL::ComPtr< IDXGISwapChain > m_swap_chain;
		};
	}
}
