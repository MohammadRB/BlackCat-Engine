// [01/23/2016 MRB]

#pragma once

#include "GraphicImp/GraphicImpPCH.h"
#include "Graphic/bcGraphicDefinition.h"
#include <d3d11.h>
#include <dxgiformat.h>

namespace black_cat
{
	namespace graphic
	{
		DXGI_FORMAT bc_graphic_cast(bc_format p_format);

		bc_format bc_graphic_cast(DXGI_FORMAT p_format);

		D3D11_FILTER bc_graphic_cast(bc_filter p_filter);

		D3D11_TEXTURE_ADDRESS_MODE bc_graphic_cast(bc_texture_address_mode p_address);

		D3D11_COMPARISON_FUNC bc_graphic_cast(bc_comparison_func p_comparison);

		D3D11_BLEND bc_graphic_cast(bc_blend p_blend);

		D3D11_BLEND_OP bc_graphic_cast(bc_blend_op p_blend_op);

		D3D11_DEPTH_WRITE_MASK bc_graphic_cast(bc_depth_write_mask p_mask);

		D3D11_STENCIL_OP bc_graphic_cast(bc_stencil_op p_stencil_op);

		D3D11_FILL_MODE bc_graphic_cast(bc_fill_mode p_fill);

		D3D11_CULL_MODE bc_graphic_cast(bc_cull_mode p_cull);

		D3D11_INPUT_CLASSIFICATION bc_graphic_cast(bc_input_classification p_classification);

		D3D11_MAP bc_graphic_cast(bc_resource_map p_map);

		D3D11_PRIMITIVE_TOPOLOGY bc_graphic_cast(bc_primitive p_primitive);

		D3D11_USAGE bc_graphic_cast(bc_resource_usage p_usage);

		bc_resource_usage bc_graphic_cast(D3D11_USAGE p_usage);
		
		void dx_call(HRESULT p_hr);
	}
}