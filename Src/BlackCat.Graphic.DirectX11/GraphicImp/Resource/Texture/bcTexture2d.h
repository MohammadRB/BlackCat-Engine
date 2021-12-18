// [01/30/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"
#include "GraphicImp/bcDeviceResourceContent.h"
#include "GraphicImp/Resource/bcResource.h"

#include "Graphic/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_texture2d_pack<g_api_dx11> : bci_platform_resource_pack<g_api_dx11>
		{
			ID3D11Texture2D* m_texture;
		};
	}
}