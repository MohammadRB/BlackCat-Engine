// [02/11/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"

#include "Graphic/Resource/View/bcIResourceView.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bci_platform_resource_view_pack<g_api_dx11> : bc_platform_device_reference_pack<g_api_dx11>
		{
		};
		
		template<>
		inline bci_platform_resource_view<g_api_dx11>::bci_platform_resource_view() noexcept = default;

		template<>
		inline bci_platform_resource_view<g_api_dx11>::bci_platform_resource_view(platform_pack& p_pack) noexcept
			: bc_platform_device_reference()
		{
		}

		template<>
		inline bci_platform_resource_view<g_api_dx11>::bci_platform_resource_view(const bci_platform_resource_view& p_other) noexcept = default;

		template<>
		inline bci_platform_resource_view<g_api_dx11>::~bci_platform_resource_view() = default;

		template<>
		inline bci_platform_resource_view<g_api_dx11>& bci_platform_resource_view<g_api_dx11>::operator=(const bci_platform_resource_view& p_other) noexcept = default;
	}
}