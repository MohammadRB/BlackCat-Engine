// [01/27/2016 MRB]

#pragma once

#include "GraphicImp/GraphicImpPCH.h"
#include "Graphic/Shader/bcShader.h"

namespace black_cat
{
	namespace graphic
	{
		Microsoft::WRL::ComPtr<ID3D10Blob> compile_shader(const bcECHAR* p_path,
			const bcCHAR* p_function_name,
			const bcCHAR* p_profile,
			const D3D_SHADER_MACRO* p_defines,
			ID3DInclude* p_includes);
	}
}
