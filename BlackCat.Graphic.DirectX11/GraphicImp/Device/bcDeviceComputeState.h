// [09/09/2016 MRB]

#pragma once

#include "GraphicImp/Shader/bcComputeShader.h"

#include "Graphic/Device/bcDeviceComputeState.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_device_compute_state_pack<bc_platform_render_api::directx11>
			: public bc_platform_device_compute_state_pack<bc_platform_render_api::unknown>
		{
		};

		template< >
		inline bc_platform_device_compute_state<bc_platform_render_api::directx11>::bc_platform_device_compute_state() = default;

		template< >
		inline bc_platform_device_compute_state<bc_platform_render_api::directx11>::bc_platform_device_compute_state(bc_platform_device_compute_state&&) noexcept = default;

		template< >
		inline bc_platform_device_compute_state<bc_platform_render_api::directx11>::~bc_platform_device_compute_state() = default;

		template< >
		inline bc_platform_device_compute_state<bc_platform_render_api::directx11>& bc_platform_device_compute_state<bc_platform_render_api::directx11>::operator=(bc_platform_device_compute_state&&) noexcept = default;
	}
}