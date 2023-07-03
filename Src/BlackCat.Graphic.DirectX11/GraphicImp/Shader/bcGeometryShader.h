// [27/01/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"
#include "GraphicImp/bcDeviceResourceContent.h"
#include "GraphicImp/Shader/bcShader.h"

#include "Graphic/Shader/bcGeometryShader.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_geometry_shader_pack<g_api_dx11> : bci_platform_shader_pack<g_api_dx11>
		{
			ID3D11GeometryShader* m_shader;
		};
	}
}