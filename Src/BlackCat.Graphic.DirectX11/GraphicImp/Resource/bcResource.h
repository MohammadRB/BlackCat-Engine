// [29/01/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"

#include "Graphic/Resource/bcResource.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat::graphic
{
	template<>
	struct bci_platform_resource_pack<g_api_dx11> : bc_platform_device_reference_pack<g_api_dx11>
	{
		ID3D11Resource* m_resource;
	};

	template<>
	inline bci_platform_resource< g_api_dx11 >::bci_platform_resource() noexcept = default;

	template<>
	inline bci_platform_resource<g_api_dx11>::bci_platform_resource(platform_pack& p_pack) noexcept
		: bc_platform_device_reference()
	{
	}

	template<>
	inline bci_platform_resource< g_api_dx11 >::~bci_platform_resource() = default;

	template<>
	inline bci_platform_resource< g_api_dx11 >::bci_platform_resource(const bci_platform_resource& p_other) noexcept = default;

	template<>
	inline bci_platform_resource< g_api_dx11 >& bci_platform_resource< g_api_dx11 >::operator=(const bci_platform_resource& p_other) noexcept = default;
}
