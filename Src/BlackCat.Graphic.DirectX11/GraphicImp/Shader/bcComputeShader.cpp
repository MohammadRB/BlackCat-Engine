// [27/01/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Shader/bcShader.h"
#include "GraphicImp/Shader/bcComputeShader.h"

using namespace Microsoft::WRL;

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_compute_shader< g_api_dx11 >::bc_platform_compute_shader() noexcept
			: bci_platform_shader()
		{
			m_pack.m_shader = nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_compute_shader<g_api_dx11>::bc_platform_compute_shader(platform_pack& p_pack) noexcept
			: bci_platform_shader(),
			m_pack(p_pack)
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_compute_shader< g_api_dx11 >::bc_platform_compute_shader(const bc_platform_compute_shader& p_other) noexcept
			: bci_platform_shader(p_other),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_compute_shader< g_api_dx11 >::~bc_platform_compute_shader()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_compute_shader< g_api_dx11 >& bc_platform_compute_shader< g_api_dx11 >::operator=(const bc_platform_compute_shader& p_other) noexcept
		{
			bci_platform_shader::operator=(p_other);
			m_pack = p_other.m_pack;

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_shader_type bc_platform_compute_shader< g_api_dx11 >::get_type() const
		{
			return bc_shader_type::compute;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_compute_shader< g_api_dx11 >::is_valid() const noexcept
		{
			return m_pack.m_shader != nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_compute_shader<g_api_dx11>::set_debug_name(const bcCHAR* p_name) noexcept
		{
			if (is_valid())
			{
				m_pack.m_shader->SetPrivateData(WKPDID_D3DDebugObjectName, std::strlen(p_name), p_name);
			}
		}
		
		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_compute_shader<g_api_dx11>::operator==(const bc_platform_compute_shader& p_other) const noexcept
		{
			return m_pack.m_shader == p_other.m_pack.m_shader;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_compute_shader<g_api_dx11>::operator!=(const bc_platform_compute_shader& p_other) const noexcept
		{
			return !operator==(p_other);
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_compute_shader<g_api_dx11>::operator==(std::nullptr_t) const noexcept
		{
			return !is_valid();
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_compute_shader<g_api_dx11>::operator!=(std::nullptr_t) const noexcept
		{
			return is_valid();
		}
	}
}