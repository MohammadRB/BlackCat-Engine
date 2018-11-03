// [01/27/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"

#include "Graphic/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcRefCountProxy.h"

using namespace Microsoft::WRL;

namespace black_cat
{
	namespace graphic
	{
		class _bc_device_command_list_proxy
		{
		public:
			ID3D11CommandList* m_command_list;
		};

		using bc_device_command_list_proxy = bc_refcount_proxy<_bc_device_command_list_proxy>;

		template< >
		struct bc_platform_device_command_list_pack<bc_render_api::directx11>
		{
			// Because in Dx11, command lists are created with a call to ID3D11DeviceContext::FinishCommandList, there is
			// no way to created command list instance with device so all copies point to the same command list object, 
			// so we used a proxy.
			bc_device_command_list_proxy* m_command_list_proxy;
		};
	}
}