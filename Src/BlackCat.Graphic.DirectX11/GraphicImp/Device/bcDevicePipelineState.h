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
		struct bc_platform_device_pipeline_state_pack< g_api_dx11 > : bc_platform_device_reference_pack<g_api_dx11>
		{
			bc_device_pipeline_state_proxy* m_pipeline_state_proxy;
		};

		template<>
		inline bc_platform_device_pipeline_state< g_api_dx11 >::bc_platform_device_pipeline_state() noexcept
			: m_pack()
		{
			m_pack.m_pipeline_state_proxy = nullptr;
		}

		template<>
		inline bc_platform_device_pipeline_state<g_api_dx11>::bc_platform_device_pipeline_state(platform_pack& p_pack) noexcept
			: m_pack()
		{
			m_pack.m_pipeline_state_proxy = p_pack.m_pipeline_state_proxy;
		}

		template<>
		inline bc_platform_device_pipeline_state< g_api_dx11 >::bc_platform_device_pipeline_state(const bc_platform_device_pipeline_state&) noexcept = default;

		template<>
		inline bc_platform_device_pipeline_state< g_api_dx11 >::~bc_platform_device_pipeline_state() = default;

		template<>
		inline bc_platform_device_pipeline_state< g_api_dx11 >& bc_platform_device_pipeline_state< g_api_dx11 >::operator=(const bc_platform_device_pipeline_state&) noexcept = default;

		template<>
		inline const bc_device_pipeline_state_config& bc_platform_device_pipeline_state< g_api_dx11 >::get_config() const
		{
			return m_pack.m_pipeline_state_proxy->m_config;
		}

		template<>
		inline bool bc_platform_device_pipeline_state< g_api_dx11 >::is_valid() const noexcept
		{
			return m_pack.m_pipeline_state_proxy != nullptr;
		}

		template<>
		inline void bc_platform_device_pipeline_state<g_api_dx11>::set_debug_name(const bcCHAR* p_name) noexcept
		{
			if (is_valid())
			{
				m_pack.m_pipeline_state_proxy->m_config.m_vertex_shader.set_debug_name(p_name);
				m_pack.m_pipeline_state_proxy->m_config.m_domain_shader.set_debug_name(p_name);
				m_pack.m_pipeline_state_proxy->m_config.m_hull_shader.set_debug_name(p_name);
				m_pack.m_pipeline_state_proxy->m_config.m_geometry_shader.set_debug_name(p_name);
				m_pack.m_pipeline_state_proxy->m_config.m_pixel_shader.set_debug_name(p_name);
				m_pack.m_pipeline_state_proxy->m_blend_state->SetPrivateData(WKPDID_D3DDebugObjectName, std::strlen(p_name), p_name);
				m_pack.m_pipeline_state_proxy->m_depth_stencil_state->SetPrivateData(WKPDID_D3DDebugObjectName, std::strlen(p_name), p_name);
				m_pack.m_pipeline_state_proxy->m_rasterizer_state->SetPrivateData(WKPDID_D3DDebugObjectName, std::strlen(p_name), p_name);
				if(m_pack.m_pipeline_state_proxy->m_input_layout)
				{
					m_pack.m_pipeline_state_proxy->m_input_layout->SetPrivateData(WKPDID_D3DDebugObjectName, std::strlen(p_name), p_name);
				}
			}
		}
		
		template<>
		inline bool bc_platform_device_pipeline_state<g_api_dx11>::operator==(const bc_platform_device_pipeline_state& p_other) const noexcept
		{
			return m_pack.m_pipeline_state_proxy == p_other.m_pack.m_pipeline_state_proxy;
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