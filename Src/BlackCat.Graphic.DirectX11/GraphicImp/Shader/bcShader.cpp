// [01/27/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"

#include "Core/Container/bcAllocator.h"
#include "CorePlatformImp/bcUtility.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Shader/bcShader.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_compiled_shader< g_api_dx11 >::bc_platform_compiled_shader()
			: bc_platform_device_reference()
		{
			m_pack.m_blob = nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_compiled_shader<g_api_dx11>::bc_platform_compiled_shader(platform_pack& p_pack)
			: bc_platform_device_reference()
		{
			m_pack.m_blob = p_pack.m_blob;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_compiled_shader< g_api_dx11 >::~bc_platform_compiled_shader()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_compiled_shader< g_api_dx11 >::bc_platform_compiled_shader(const bc_platform_compiled_shader& p_other)
			: bc_platform_device_reference(p_other)
		{
			m_pack.m_blob = p_other.m_pack.m_blob;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_compiled_shader< g_api_dx11 >& bc_platform_compiled_shader< g_api_dx11 >::operator=(const bc_platform_compiled_shader& p_other)
		{
			bc_platform_device_reference::operator=(std::move(p_other));
			m_pack.m_blob = p_other.m_pack.m_blob;

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_compiled_shader<g_api_dx11>::is_valid() const noexcept
		{
			return m_pack.m_blob != nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_compiled_shader<g_api_dx11>::set_debug_name(const bcCHAR* p_name) noexcept
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_ishader< g_api_dx11 >::bc_platform_ishader()
			: bc_platform_device_reference()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_ishader<g_api_dx11>::bc_platform_ishader(platform_pack& p_pack)
			: bc_platform_device_reference()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_ishader< g_api_dx11 >::~bc_platform_ishader()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_ishader< g_api_dx11 >::bc_platform_ishader(const bc_platform_ishader& p_other)
			: bc_platform_device_reference(p_other)
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_ishader< g_api_dx11 >& bc_platform_ishader< g_api_dx11 >::operator=(const bc_platform_ishader& p_other)
		{
			bc_platform_device_reference::operator=(p_other);

			return *this;
		}
	}
}