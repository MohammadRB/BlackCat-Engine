// [01/27/2016 MRB]

#pragma once

#include "GraphicImp/Shader/bcShader.h"

#include "Graphic/Shader/bcGeometryShader.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_geometry_shader_pack<bc_render_api::directx11>
		{
			Microsoft::WRL::ComPtr< ID3D11GeometryShader > m_shader;
		};
	}
}