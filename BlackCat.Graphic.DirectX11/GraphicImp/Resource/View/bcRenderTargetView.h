// [02/14/2016 MRB]

#pragma once

#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/View/bcRenderTargetViewConfig.h"

#include "Graphic/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		struct bc_platform_render_target_view_pack< g_api_dx11 >
			: public bc_platform_render_target_view_pack< g_api_unknown >
		{
			Microsoft::WRL::ComPtr< ID3D11RenderTargetView > m_render_target_view;
		};
	}
}
