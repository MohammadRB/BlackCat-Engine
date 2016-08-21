// [01/29/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcUtility.h"
#include "CorePlatformImp/bcUtility.h"

namespace black_cat
{
	namespace graphic
	{
		DXGI_FORMAT bc_graphic_cast(bc_format p_format)
		{
			return static_cast<DXGI_FORMAT>(p_format);
		}

		bc_format bc_graphic_cast(DXGI_FORMAT p_format)
		{
			return static_cast<bc_format>(p_format);
		}

		D3D11_FILTER bc_graphic_cast(bc_filter p_filter)
		{
			return static_cast<D3D11_FILTER>(p_filter);
		}

		D3D11_TEXTURE_ADDRESS_MODE bc_graphic_cast(bc_texture_address_mode p_address)
		{
			return static_cast<D3D11_TEXTURE_ADDRESS_MODE>(p_address);
		}

		D3D11_COMPARISON_FUNC bc_graphic_cast(bc_comparison_func p_comparison)
		{
			return static_cast<D3D11_COMPARISON_FUNC>(p_comparison);
		}

		D3D11_BLEND bc_graphic_cast(bc_blend p_blend)
		{
			return static_cast<D3D11_BLEND>(p_blend);
		}

		D3D11_BLEND_OP bc_graphic_cast(bc_blend_op p_blend_op)
		{
			return static_cast<D3D11_BLEND_OP>(p_blend_op);
		}

		D3D11_DEPTH_WRITE_MASK bc_graphic_cast(bc_depth_write_mask p_mask)
		{
			return static_cast<D3D11_DEPTH_WRITE_MASK>(p_mask);
		}

		D3D11_STENCIL_OP bc_graphic_cast(bc_stencil_op p_stencil_op)
		{
			return static_cast<D3D11_STENCIL_OP>(p_stencil_op);
		}

		D3D11_FILL_MODE bc_graphic_cast(bc_fill_mode p_fill)
		{
			return static_cast<D3D11_FILL_MODE>(p_fill);
		}

		D3D11_CULL_MODE bc_graphic_cast(bc_cull_mode p_cull)
		{
			return static_cast<D3D11_CULL_MODE>(p_cull);
		}

		D3D11_INPUT_CLASSIFICATION bc_graphic_cast(bc_input_classification p_classification)
		{
			return static_cast<D3D11_INPUT_CLASSIFICATION>(p_classification);
		}

		D3D11_MAP bc_graphic_cast(bc_resource_map p_map)
		{
			return static_cast<D3D11_MAP>(p_map);
		}

		D3D11_PRIMITIVE_TOPOLOGY bc_graphic_cast(bc_primitive p_primitive)
		{
			return static_cast<D3D11_PRIMITIVE_TOPOLOGY>(p_primitive);
		}

		D3D11_USAGE bc_graphic_cast(bc_resource_usage p_usage)
		{
			return static_cast<D3D11_USAGE>(p_usage);
		}

		bc_resource_usage bc_graphic_cast(D3D11_USAGE p_usage)
		{
			return static_cast<bc_resource_usage>(p_usage);
		}

		void dx_call(HRESULT p_hr)
		{
			if (FAILED(p_hr))
			{
				bcCHAR l_error_string[MAX_PATH];
				FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
					0,
					p_hr,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					l_error_string,
					MAX_PATH,
					NULL);

				core::bc_string l_message = "DirectX Error: ";
				l_message += l_error_string;

				DWORD l_win32_code;
				core_platform::win32_from_hresult(p_hr, &l_win32_code);

				throw bc_graphic_exception(static_cast<bcINT>(l_win32_code),l_message);
			}
		}
	}
}