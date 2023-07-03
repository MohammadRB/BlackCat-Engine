// [11/02/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"
#include "GraphicImp/Resource/View/bcIResourceView.h"
#include "GraphicImp/Resource/View/bcResourceViewConfig.h"

#include "Graphic/Resource/View/bcResourceView.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_shader_view_pack<g_api_dx11> : bci_platform_resource_view_pack<g_api_dx11>
		{
			ID3D11ShaderResourceView* m_shader_view;
			ID3D11UnorderedAccessView* m_unordered_shader_view;
		};
	}
}