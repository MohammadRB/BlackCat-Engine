// [01/27/2016 MRB]

#pragma once

#include "GraphicImp/GraphicImpPCH.h"
#include "Graphic/Shader/bcShader.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		struct bc_platform_compiled_shader_pack< g_api_dx11 >
		{
		public:
			Microsoft::WRL::ComPtr< ID3D10Blob > m_blob;
		};
	}
}
