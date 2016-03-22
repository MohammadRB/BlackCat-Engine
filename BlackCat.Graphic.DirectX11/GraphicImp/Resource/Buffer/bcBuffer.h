// [01/29/2016 MRB]

#pragma once

#include "GraphicImp/Resource/bcResource.h"
#include "GraphicImp/Resource/Buffer/bcBufferConfig.h"

#include "Graphic/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_buffer_pack<g_api_dx11> : public bc_platform_buffer_pack<g_api_unknown>
		{
			Microsoft::WRL::ComPtr< ID3D11Buffer > m_buffer;
		};
	}
}
