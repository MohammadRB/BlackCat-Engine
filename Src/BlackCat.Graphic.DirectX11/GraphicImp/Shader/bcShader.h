// [27/01/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"

#include "Graphic/Shader/bcShader.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_compiled_shader_pack<g_api_dx11> : bc_platform_device_reference_pack<g_api_dx11>
		{
		public:
			ID3D10Blob* m_blob;
		};

		template<>
		struct bci_platform_shader_pack<g_api_dx11> : bc_platform_device_reference_pack<g_api_dx11>
		{
		};

		template<>
		inline bc_platform_compiled_shader<g_api_dx11>::bc_platform_compiled_shader() noexcept
			: bc_platform_device_reference()
		{
			m_pack.m_blob = nullptr;
		}

		template<>
		inline bc_platform_compiled_shader<g_api_dx11>::bc_platform_compiled_shader(platform_pack& p_pack) noexcept
			: bc_platform_device_reference(p_pack),
			m_pack(p_pack)
		{
		}

		template<>
		inline bc_platform_compiled_shader<g_api_dx11>::~bc_platform_compiled_shader() noexcept = default;

		template<>
		inline bc_platform_compiled_shader<g_api_dx11>::bc_platform_compiled_shader(const bc_platform_compiled_shader& p_other) noexcept = default;

		template<>
		inline bc_platform_compiled_shader<g_api_dx11>& bc_platform_compiled_shader<g_api_dx11>::operator=(const bc_platform_compiled_shader & p_other) noexcept = default;
		
		template<>
		inline bci_platform_shader<g_api_dx11>::bci_platform_shader() noexcept = default;

		template<>
		inline bci_platform_shader<g_api_dx11>::bci_platform_shader(platform_pack& p_pack) noexcept
			: bc_platform_device_reference()
		{
		}

		template<>
		inline bci_platform_shader<g_api_dx11>::bci_platform_shader(const bci_platform_shader& p_other) noexcept = default;
		
		template<>
		inline bci_platform_shader<g_api_dx11>::~bci_platform_shader() = default;

		template<>
		inline bci_platform_shader<g_api_dx11>& bci_platform_shader<g_api_dx11>::operator=(const bci_platform_shader& p_other) noexcept = default;
	}
}