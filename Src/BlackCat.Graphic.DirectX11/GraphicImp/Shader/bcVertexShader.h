// [27/01/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"
#include "GraphicImp/bcDeviceResourceContent.h"
#include "GraphicImp/Shader/bcShader.h"

#include "Graphic/Shader/bcVertexShader.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat::graphic
{
	template<>
	struct bc_platform_vertex_shader_pack<g_api_dx11> : bci_platform_shader_pack<g_api_dx11>
	{
		ID3DBlob* m_compiled_shader;
		ID3D11VertexShader* m_shader;
	};
}
