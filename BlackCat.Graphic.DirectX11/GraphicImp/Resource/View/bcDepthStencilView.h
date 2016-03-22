// [02/14/2016 MRB]

#pragma once

#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/View/bcDepthStencilViewConfig.h"

#include "Graphic/Resource/View/bcDepthStencilView.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		struct bc_platform_depth_stencil_view_pack< g_api_dx11 >
			: public bc_platform_depth_stencil_view_pack< g_api_unknown >
		{
			Microsoft::WRL::ComPtr< ID3D11DepthStencilView > m_depth_stencil_view;
		};
	}
}
