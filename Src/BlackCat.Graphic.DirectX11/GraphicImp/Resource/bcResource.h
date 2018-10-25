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
		struct bc_platform_iresource_pack<g_api_dx11>
		{
			bc_platform_iresource_pack()
				: m_resource(nullptr)
			{
			}

			explicit bc_platform_iresource_pack(ID3D11Resource* p_resource)
				: m_resource(p_resource)
			{
			}

			ID3D11Resource* m_resource;
		};
	}
}