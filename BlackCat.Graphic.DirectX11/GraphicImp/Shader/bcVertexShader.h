// [01/27/2016 MRB]

#pragma once

#include "GraphicImp/Shader/bcShader.h"

#include "Graphic/Shader/bcVertexShader.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_vertex_shader_pack<bc_render_api::directx11>
		{
			Microsoft::WRL::ComPtr< ID3DBlob > m_compiled_shader;
			Microsoft::WRL::ComPtr< ID3D11VertexShader > m_shader;
		};
	}
}