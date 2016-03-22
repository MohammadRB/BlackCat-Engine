// [03/30/2016 MRB]

#pragma once

#include "Graphic/Resource/View/bcDepthStencilViewConfig.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		struct bc_platform_depth_stencil_view_config_pack< g_api_dx11 >
			: public bc_platform_depth_stencil_view_config_pack<g_api_unknown>
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC m_depth_stencil_view_desc;
		};
	}
}