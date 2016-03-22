// [01/30/2016 MRB]

#pragma once

#include "GraphicImp/Resource/bcResource.h"
#include "GraphicImp/Resource/Texture/bcTextureConfig.h"

#include "Graphic/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		struct bc_platform_texture2d_pack<g_api_dx11>
			: public bc_platform_texture2d_pack<g_api_unknown>
		{
			Microsoft::WRL::ComPtr< ID3D11Texture2D > m_texture;
		};
	}
}