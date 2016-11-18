// [01/27/2016 MRB]

#pragma once

#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"

#include "Graphic/Device/Command/bcDeviceCommandExecuter.h"
#include "GraphicImp/GraphicImpPCH.h"

using namespace Microsoft::WRL;

namespace black_cat
{
	namespace graphic
	{
		template< >
		struct bc_platform_device_command_executer_pack<bc_platform_render_api::directx11>
		{
		};
	}
}
