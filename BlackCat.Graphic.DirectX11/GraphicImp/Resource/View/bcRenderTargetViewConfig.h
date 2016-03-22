// [03/30/2016 MRB]

#pragma once

#include "Graphic/Resource/View/bcRenderTargetViewConfig.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		struct bc_platform_render_target_view_config_pack< g_api_dx11 >
			: public bc_platform_render_target_view_config_pack<g_api_unknown>
		{
				D3D11_RENDER_TARGET_VIEW_DESC m_render_target_view_desc;
		};
	}
}