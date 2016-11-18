// [01/29/2016 MRB]

#pragma once

#include "Graphic/Resource/bcResource.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_iresource_pack<g_api_dx11>
		{
			Microsoft::WRL::ComPtr< ID3D11Resource > m_resource;
		};
	}
}
