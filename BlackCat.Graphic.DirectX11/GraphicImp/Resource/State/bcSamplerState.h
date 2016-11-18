// [01/25/2016 MRB]

#pragma once

#include "Graphic/Resource/State/bcSamplerState.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_sampler_state_pack<bc_platform_render_api::directx11>
		{
			Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler_state;
		};

		template< >
		inline bc_platform_sampler_state<bc_platform_render_api::directx11>::bc_platform_sampler_state() = default;

		template<  >
		inline bc_platform_sampler_state<bc_platform_render_api::directx11>::bc_platform_sampler_state(bc_platform_sampler_state&& p_other) = default;

		template< >
		inline bc_platform_sampler_state<bc_platform_render_api::directx11>::~bc_platform_sampler_state() = default;

		template<  >
		inline bc_platform_sampler_state<bc_platform_render_api::directx11>& bc_platform_sampler_state<bc_platform_render_api::directx11>::operator=(bc_platform_sampler_state&& p_other) = default;
	}
}