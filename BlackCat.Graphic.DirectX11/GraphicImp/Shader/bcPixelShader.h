// [01/27/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"
#include "GraphicImp/bcDeviceResourceContent.h"
#include "GraphicImp/Shader/bcShader.h"

#include "Graphic/Shader/bcPixelShader.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_pixel_shader_pack<bc_render_api::directx11>
		{
			ID3D11PixelShader* m_shader;
		};
	}
}