// [02/12/2016 MRB]

#pragma once

#include "Graphic/Resource/Texture/bcTextureConfig.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		struct bc_platform_texture_config_pack< g_api_dx11 >
			: public bc_platform_texture_config_pack< g_api_unknown >
		{
			D3D11_TEXTURE2D_DESC m_desc;
		};
	}
}