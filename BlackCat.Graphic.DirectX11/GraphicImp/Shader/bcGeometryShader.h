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
		struct bc_platform_geometry_shader_pack<bc_platform_render_api::directx11>
			: public bc_platform_geometry_shader_pack<bc_platform_render_api::unknown>
		{
			Microsoft::WRL::ComPtr< ID3D11GeometryShader > m_shader;
		};
	}
}