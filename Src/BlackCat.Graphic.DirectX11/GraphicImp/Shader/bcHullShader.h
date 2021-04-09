// [01/27/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"
#include "GraphicImp/bcDeviceResourceContent.h"
#include "GraphicImp/Shader/bcShader.h"

#include "Graphic/Shader/bcHullShader.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_hull_shader_pack<g_api_dx11> : bci_platform_shader_pack<g_api_dx11>
		{
			ID3D11HullShader* m_shader;
		};
	}
}