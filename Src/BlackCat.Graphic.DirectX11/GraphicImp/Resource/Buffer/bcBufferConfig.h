// [12/02/2016 MRB]

#pragma once

#include "Graphic/Resource/Buffer/bcBufferConfig.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat::graphic
{
	template< >
	struct bc_platform_buffer_config_pack< g_api_dx11 >
			: public bc_platform_buffer_config_pack< g_api_unknown >
	{
		D3D11_BUFFER_DESC m_desc;
	};
}
