// [01/27/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"

#include "Graphic/Shader/bcShader.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		struct bc_platform_compiled_shader_pack< g_api_dx11 >
		{
		public:
			ID3D10Blob* m_blob;
		};
	}
}
