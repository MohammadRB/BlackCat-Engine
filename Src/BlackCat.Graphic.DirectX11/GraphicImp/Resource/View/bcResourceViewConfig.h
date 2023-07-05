// [12/02/2016 MRB]

#pragma once

#include "Graphic/Resource/View/bcResourceViewConfig.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat::graphic
{
	template< >
	struct bc_platform_resource_view_config_pack< g_api_dx11 >
			: public bc_platform_resource_view_config_pack<g_api_unknown>
	{
		union
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC m_shader_view_desc;
			D3D11_UNORDERED_ACCESS_VIEW_DESC m_unordered_shader_view_desc;
		};
	};
}
