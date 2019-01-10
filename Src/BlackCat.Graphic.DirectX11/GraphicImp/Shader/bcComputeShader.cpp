// [01/27/2016 MRB]

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
		bc_platform_compute_shader< g_api_dx11 >::bc_platform_compute_shader()
			: bc_platform_ishader()
		{
			m_pack.m_shader = nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_compute_shader<g_api_dx11>::bc_platform_compute_shader(platform_pack& p_pack)
			: bc_platform_ishader()
		{
			m_pack.m_shader = p_pack.m_shader;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_compute_shader< g_api_dx11 >::~bc_platform_compute_shader()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_compute_shader< g_api_dx11 >::bc_platform_compute_shader(const bc_platform_compute_shader& p_other)
			: bc_platform_ishader(p_other)
		{
			m_pack.m_shader = p_other.m_pack.m_shader;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_compute_shader< g_api_dx11 >& bc_platform_compute_shader< g_api_dx11 >::operator=(const bc_platform_compute_shader& p_other)
		{
			bc_platform_ishader::operator=(p_other);
			m_pack.m_shader = p_other.m_pack.m_shader;

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