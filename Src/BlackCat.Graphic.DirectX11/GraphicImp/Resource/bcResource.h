// [01/29/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"

#include "Graphic/Resource/bcResource.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bci_platform_resource_pack<g_api_dx11>
		{
			bci_platform_resource_pack()
				: m_resource(nullptr)
			{
			}

			explicit bci_platform_resource_pack(ID3D11Resource* p_resource)
				: m_resource(p_resource)
			{
			}

			ID3D11Resource* m_resource;
		};
	}
}