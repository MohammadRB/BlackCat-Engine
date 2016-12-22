// [01/25/2016 MRB]

#pragma once

#include "GraphicImp/Shader/bcVertexShader.h"
#include "GraphicImp/Shader/bcHullShader.h"
#include "GraphicImp/Shader/bcDomainShader.h"
#include "GraphicImp/Shader/bcGeometryShader.h"
#include "GraphicImp/Shader/bcPixelShader.h"
#include "GraphicImp/Shader/bcComputeShader.h"

#include "Graphic/Device/bcDevicePipelineState.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_device_pipeline_state_pack<bc_render_api::directx11>
			: public bc_platform_device_pipeline_state_pack<bc_render_api::unknown>
		{
			Microsoft::WRL::ComPtr< ID3D11BlendState > m_blend_state;
			Microsoft::WRL::ComPtr< ID3D11DepthStencilState > m_depth_stencil_state;
			Microsoft::WRL::ComPtr< ID3D11RasterizerState > m_rasterizer_state;
			Microsoft::WRL::ComPtr< ID3D11InputLayout > m_input_layout;
		};

		template< >
		inline bc_platform_device_pipeline_state<bc_render_api::directx11>::bc_platform_device_pipeline_state() = default;

		template< >
		inline bc_platform_device_pipeline_state<bc_render_api::directx11>::bc_platform_device_pipeline_state(bc_platform_device_pipeline_state&&) = default;

		template< >
		inline bc_platform_device_pipeline_state<bc_render_api::directx11>::~bc_platform_device_pipeline_state() = default;

		template< >
		inline bc_platform_device_pipeline_state<bc_render_api::directx11>& bc_platform_device_pipeline_state<bc_render_api::directx11>::operator=(bc_platform_device_pipeline_state&&) = default;
	}
}

