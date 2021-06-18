// [01/23/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceRef.h"
#include "GraphicImp/Device/bcDeviceOutput.h"
#include "GraphicImp/Resource/bcResource.h"

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Graphic/Device/bcDevice.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_device_pack<bc_render_api::directx11>
		{
			Microsoft::WRL::ComPtr<ID3D11Device> m_device;
			Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_immediate_context;
			core_platform::bc_mutex m_immediate_context_mutex;
		};
	}
}