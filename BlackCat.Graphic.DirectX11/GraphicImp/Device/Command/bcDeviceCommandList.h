// [01/27/2016 MRB]

#pragma once

#include "Graphic/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/GraphicImpPCH.h"

using namespace Microsoft::WRL;

namespace black_cat
{
	namespace graphic
	{
		template< >
		struct bc_platform_device_command_list_pack<bc_platform_render_api::directx11>
			: public bc_platform_device_command_list_pack<bc_platform_render_api::unknown>
		{
			ComPtr< ID3D11CommandList > m_command_list;
		};
	}
}