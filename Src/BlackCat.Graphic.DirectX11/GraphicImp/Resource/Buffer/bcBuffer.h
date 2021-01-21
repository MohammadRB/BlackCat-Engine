// [01/29/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceRef.h"
#include "GraphicImp/Resource/bcResource.h"
#include "GraphicImp/Resource/Buffer/bcBufferConfig.h"

#include "Graphic/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_buffer_pack<g_api_dx11>
		{
			ID3D11Buffer* m_buffer;
		};
	}
}