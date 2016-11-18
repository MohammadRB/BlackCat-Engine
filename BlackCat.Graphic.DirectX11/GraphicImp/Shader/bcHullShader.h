// [01/27/2016 MRB]

#pragma once

#include "GraphicImp/Shader/bcShader.h"

#include "Graphic/Shader/bcHullShader.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_hull_shader_pack<bc_platform_render_api::directx11>
		{
			Microsoft::WRL::ComPtr< ID3D11HullShader > m_shader;
		};
	}
}