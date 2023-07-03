// [14/02/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"
#include "GraphicImp/Resource/View/bcIResourceView.h"
#include "GraphicImp/Resource/View/bcRenderTargetViewConfig.h"

#include "Graphic/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_render_target_view_pack<g_api_dx11> : bci_platform_resource_view_pack<g_api_dx11>
		{
			ID3D11RenderTargetView* m_render_target_view;
		};
	}
}