// [22/12/2016 MRB]

#pragma once

#include "Graphic/bcDeviceReference.h"
#include "GraphicImp/bcExport.h"

namespace black_cat::graphic
{
	template<>
	struct bc_platform_device_reference_pack<g_api_dx11>
	{
	};

	template<>
	BC_GRAPHICIMP_DLL
	inline bc_platform_device_reference<g_api_dx11>::~bc_platform_device_reference() = default;

	template<>
	BC_GRAPHICIMP_DLL
	inline bc_platform_device_reference<g_api_dx11>::bc_platform_device_reference() noexcept = default;

	template<>
	BC_GRAPHICIMP_DLL
	inline bc_platform_device_reference<g_api_dx11>::bc_platform_device_reference(platform_pack& p_pack) noexcept
	{
	}

	template<>
	BC_GRAPHICIMP_DLL
	inline bc_platform_device_reference<g_api_dx11>::bc_platform_device_reference(const bc_platform_device_reference&) noexcept = default;

	template<>
	BC_GRAPHICIMP_DLL
	inline bc_platform_device_reference<g_api_dx11>& bc_platform_device_reference<g_api_dx11>::operator=(const bc_platform_device_reference&) noexcept = default;
}
