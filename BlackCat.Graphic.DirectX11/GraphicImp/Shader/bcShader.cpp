// [01/27/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "Core/Container/bcAllocator.h"
#include "CorePlatformImp/bcUtility.h"
#include "GraphicImp/Shader/bcShader.h"

using namespace Microsoft::WRL;

namespace black_cat
{
	namespace graphic
	{
		template< >
		BC_GRAPHICIMP_DLL
		bc_platform_compiled_shader< g_api_dx11 >::bc_platform_compiled_shader()
			: m_pack()
		{
		}

		template< >
		BC_GRAPHICIMP_DLL
		bc_platform_compiled_shader< g_api_dx11 >::~bc_platform_compiled_shader()
		{
		}

		template< >
		BC_GRAPHICIMP_DLL
		bc_platform_compiled_shader< g_api_dx11 >::bc_platform_compiled_shader(bc_platform_compiled_shader&& p_other)
			: bc_device_object(std::move(p_other)),
			m_pack(std::move(p_other.m_pack))
		{
		}

		template< >
		BC_GRAPHICIMP_DLL
		bc_platform_compiled_shader< g_api_dx11 >& bc_platform_compiled_shader<bc_platform_render_api::directx11>::operator=(bc_platform_compiled_shader&& p_other)
		{
			bc_device_object::operator=(std::move(p_other));
			m_pack = std::move(p_other.m_pack);

			return *this;
		}

		template< >
		BC_GRAPHICIMP_DLL 
		bc_platform_ishader<bc_platform_render_api::directx11>::bc_platform_ishader()
			: m_pack()
		{
		}

		template< >
		BC_GRAPHICIMP_DLL 
		bc_platform_ishader< bc_platform_render_api::directx11 >::~bc_platform_ishader()
		{
		}

		template< >
		BC_GRAPHICIMP_DLL 
		bc_platform_ishader<bc_platform_render_api::directx11>::bc_platform_ishader(bc_platform_ishader&& p_other)
			: bc_device_object(std::move(p_other)),
			m_pack(std::move(p_other.m_pack))
		{
		}

		template< >
		BC_GRAPHICIMP_DLL 
		bc_platform_ishader<bc_platform_render_api::directx11>& bc_platform_ishader<bc_platform_render_api::directx11>::operator=(bc_platform_ishader&& p_other)
		{
			bc_device_object::operator=(std::move(p_other));
			m_pack = std::move(p_other.m_pack);

			return *this;
		}
	}
}