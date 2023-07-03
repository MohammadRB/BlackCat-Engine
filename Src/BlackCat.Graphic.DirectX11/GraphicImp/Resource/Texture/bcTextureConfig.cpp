// [17/03/2017 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Resource/Texture/bcTextureConfig.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bcUINT32 bc_platform_texture_config<g_api_dx11>::get_width() const noexcept
		{
			return m_pack.m_desc.Width;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bcUINT32 bc_platform_texture_config<g_api_dx11>::get_height() const noexcept
		{
			return m_pack.m_desc.Height;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_resource_usage bc_platform_texture_config<g_api_dx11>::get_usage() const noexcept
		{
			return bc_graphic_cast(m_pack.m_desc.Usage);
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_texture_ms_config bc_platform_texture_config<g_api_dx11>::get_sample_count() const noexcept
		{
			return bc_texture_ms_config(m_pack.m_desc.SampleDesc.Count, m_pack.m_desc.SampleDesc.Quality);
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_format bc_platform_texture_config<g_api_dx11>::get_format() const noexcept
		{
			return bc_graphic_cast(m_pack.m_desc.Format);
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_resource_view_type bc_platform_texture_config<g_api_dx11>::get_view_types() const noexcept
		{
			return bc_graphic_cast(static_cast<D3D11_BIND_FLAG>(m_pack.m_desc.BindFlags));
		}
	}
}