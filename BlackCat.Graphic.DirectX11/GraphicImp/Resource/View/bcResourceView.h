// [02/11/2016 MRB]

#pragma once

#include "GraphicImp/Resource/View/bcIResourceView.h"
#include "GraphicImp/Resource/View/bcResourceViewConfig.h"

#include "Graphic/Resource/View/bcResourceView.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		struct bc_platform_shader_view_pack< g_api_dx11 >
			: public bc_platform_shader_view_pack< g_api_unknown >
		{
			Microsoft::WRL::ComPtr< ID3D11ShaderResourceView > m_shader_view;
			Microsoft::WRL::ComPtr< ID3D11UnorderedAccessView > m_unordered_shader_view;
		};
	}
}
