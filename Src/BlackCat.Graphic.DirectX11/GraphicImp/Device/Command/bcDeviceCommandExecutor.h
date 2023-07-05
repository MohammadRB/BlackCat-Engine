// [27/01/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"

#include "Graphic/Device/Command/bcDeviceCommandExecutor.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat::graphic
{
	template< >
	struct bc_platform_device_command_executor_pack<g_api_dx11> : bc_platform_device_reference_pack<g_api_dx11>
	{
		bc_device* m_device;
	};
}
