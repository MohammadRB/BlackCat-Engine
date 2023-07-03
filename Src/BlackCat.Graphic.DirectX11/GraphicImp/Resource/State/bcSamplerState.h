// [25/01/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"

#include "Graphic/Resource/State/bcSamplerState.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_sampler_state_pack<g_api_dx11> : bc_platform_device_reference_pack<g_api_dx11>
		{
			ID3D11SamplerState* m_sampler_state;
		};

		template<>
		inline bc_platform_sampler_state<g_api_dx11>::bc_platform_sampler_state() noexcept
			: bc_platform_device_reference()
		{
			m_pack.m_sampler_state = nullptr;
		}

		template<>
		inline bc_platform_sampler_state<g_api_dx11>::bc_platform_sampler_state(platform_pack& p_pack) noexcept
			: bc_platform_device_reference(p_pack),
			m_pack(p_pack)
		{
		}

		template<>
		inline bc_platform_sampler_state< g_api_dx11 >::bc_platform_sampler_state(const bc_platform_sampler_state& p_other) noexcept = default;

		template<>
		inline bc_platform_sampler_state< g_api_dx11 >::~bc_platform_sampler_state() = default;

		template<>
		inline bc_platform_sampler_state< g_api_dx11 >& bc_platform_sampler_state< g_api_dx11 >::operator=(const bc_platform_sampler_state& p_other) noexcept = default;

		template<>
		inline bool bc_platform_sampler_state< g_api_dx11 >::is_valid() const noexcept
		{
			return m_pack.m_sampler_state != nullptr;
		}

		template<>
		inline void bc_platform_sampler_state<g_api_dx11>::set_debug_name(const bcCHAR* p_name) noexcept
		{
			if (is_valid())
			{
				m_pack.m_sampler_state->SetPrivateData(WKPDID_D3DDebugObjectName, std::strlen(p_name), p_name);
			}
		}

		template<>
		inline bool bc_platform_sampler_state<g_api_dx11>::operator==(const bc_platform_sampler_state& p_other) const noexcept
		{
			return m_pack.m_sampler_state == p_other.m_pack.m_sampler_state;
		}

		template<>
		inline bool bc_platform_sampler_state<g_api_dx11>::operator!=(const bc_platform_sampler_state& p_other) const noexcept
		{
			return !operator==(p_other);
		}

		template<>
		inline bool bc_platform_sampler_state<g_api_dx11>::operator==(std::nullptr_t) const noexcept
		{
			return !is_valid();
		}

		template<>
		inline bool bc_platform_sampler_state<g_api_dx11>::operator!=(std::nullptr_t) const noexcept
		{
			return is_valid();
		}
	}
}