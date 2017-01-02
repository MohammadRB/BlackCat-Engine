// [01/25/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"
#include "GraphicImp/Shader/bcVertexShader.h"
#include "GraphicImp/Shader/bcHullShader.h"
#include "GraphicImp/Shader/bcDomainShader.h"
#include "GraphicImp/Shader/bcGeometryShader.h"
#include "GraphicImp/Shader/bcPixelShader.h"

#include "Graphic/Device/bcDevicePipelineState.h"
#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcRefCountProxy.h"

namespace black_cat
{
	namespace graphic
	{
		class _bc_device_pipeline_state
		{
		public:
			bc_device_pipeline_state_config m_config;
			ID3D11BlendState* m_blend_state;
			ID3D11DepthStencilState* m_depth_stencil_state;
			ID3D11RasterizerState* m_rasterizer_state;
			ID3D11InputLayout* m_input_layout;
		};

		using bc_device_pipeline_state_proxy = bc_refcount_proxy<_bc_device_pipeline_state>;

		template<>
		struct bc_platform_device_pipeline_state_pack< g_api_dx11 >
		{
			bc_device_pipeline_state_proxy* m_pipeline_state;
		};

		template<>
		inline bc_platform_device_pipeline_state< g_api_dx11 >::bc_platform_device_pipeline_state()
		{
			m_pack.m_pipeline_state = nullptr;
		}

		template<>
		inline bc_platform_device_pipeline_state<g_api_dx11>::bc_platform_device_pipeline_state(platform_pack& p_pack)
		{
			m_pack.m_pipeline_state = p_pack.m_pipeline_state;
		}

		template<>
		inline bc_platform_device_pipeline_state< g_api_dx11 >::bc_platform_device_pipeline_state(const bc_platform_device_pipeline_state&) = default;

		template<>
		inline bc_platform_device_pipeline_state< g_api_dx11 >::~bc_platform_device_pipeline_state() = default;

		template<>
		inline bc_platform_device_pipeline_state< g_api_dx11 >& bc_platform_device_pipeline_state< g_api_dx11 >::operator=(const bc_platform_device_pipeline_state&) = default;

		template<>
		inline void bc_platform_device_pipeline_state< g_api_dx11 >::get_config(bc_device_pipeline_state_config& p_config)
		{
			p_config = m_pack.m_pipeline_state->m_config;
		}

		template<>
		inline bool bc_platform_device_pipeline_state< g_api_dx11 >::is_valid() const noexcept
		{
			return m_pack.m_pipeline_state != nullptr;
		}

		template<>
		inline bool bc_platform_device_pipeline_state<g_api_dx11>::operator==(const bc_platform_device_pipeline_state& p_other) const noexcept
		{
			return m_pack.m_pipeline_state == p_other.m_pack.m_pipeline_state;
		}

		template<>
		inline bool bc_platform_device_pipeline_state<g_api_dx11>::operator!=(const bc_platform_device_pipeline_state& p_other) const noexcept
		{
			return !operator==(p_other);
		}

		template<>
		inline bool bc_platform_device_pipeline_state<g_api_dx11>::operator==(std::nullptr_t) const noexcept
		{
			return !is_valid();
		}

		template<>
		inline bool bc_platform_device_pipeline_state<g_api_dx11>::operator!=(std::nullptr_t) const noexcept
		{
			return is_valid();
		}
	}
}