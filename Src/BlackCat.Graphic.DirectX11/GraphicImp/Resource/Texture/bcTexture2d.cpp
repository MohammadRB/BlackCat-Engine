// [01/30/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "PlatformImp/File/bcPath.h"
#include "GraphicImp/Resource/bcResource.h"
#include "GraphicImp/Resource/Texture/bcTextureConfig.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Device/bcDevice.h"

using namespace Microsoft::WRL;

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_texture2d< g_api_dx11 >::bc_platform_texture2d()
			: bc_platform_iresource()
		{
			m_pack.m_texture = nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_texture2d<g_api_dx11>::bc_platform_texture2d(platform_pack& p_pack)
			: bc_platform_iresource(bc_platform_iresource::platform_pack(p_pack.m_texture))
		{
			m_pack.m_texture = p_pack.m_texture;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_texture2d< g_api_dx11 >::~bc_platform_texture2d()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_texture2d< g_api_dx11 >::bc_platform_texture2d(const bc_platform_texture2d& p_other)
			: bc_platform_iresource(p_other)
		{
			m_pack.m_texture = p_other.m_pack.m_texture;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_texture2d< g_api_dx11 >& bc_platform_texture2d< g_api_dx11 >::operator=(const bc_platform_texture2d& p_other)
		{
			bc_platform_iresource::operator=(p_other);
			m_pack.m_texture = p_other.m_pack.m_texture;

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bcUINT32 bc_platform_texture2d< g_api_dx11 >::get_width() const
		{
			D3D11_TEXTURE2D_DESC l_desc;

			m_pack.m_texture->GetDesc(&l_desc);

			return l_desc.Width;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bcUINT32 bc_platform_texture2d< g_api_dx11 >::get_height() const
		{
			D3D11_TEXTURE2D_DESC l_desc;

			m_pack.m_texture->GetDesc(&l_desc);

			return l_desc.Height;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_resource_usage bc_platform_texture2d< g_api_dx11 >::get_usage() const
		{
			D3D11_TEXTURE2D_DESC l_desc;

			m_pack.m_texture->GetDesc(&l_desc);

			return bc_graphic_cast(l_desc.Usage);
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_texture2d< g_api_dx11 >::is_multisampled() const
		{
			D3D11_TEXTURE2D_DESC l_desc;

			m_pack.m_texture->GetDesc(&l_desc);

			return l_desc.SampleDesc.Count > 1;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_texture_ms_config bc_platform_texture2d< g_api_dx11 >::get_sample_count() const
		{
			D3D11_TEXTURE2D_DESC l_desc;

			m_pack.m_texture->GetDesc(&l_desc);

			return bc_texture_ms_config(l_desc.SampleDesc.Count, l_desc.SampleDesc.Quality);
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_format bc_platform_texture2d< g_api_dx11 >::get_format() const
		{
			D3D11_TEXTURE2D_DESC l_desc;

			m_pack.m_texture->GetDesc(&l_desc);

			return bc_graphic_cast(l_desc.Format);
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_texture2d<g_api_dx11>::is_valid() const noexcept
		{
			return m_pack.m_texture != nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_texture2d<g_api_dx11>::operator==(const bc_platform_texture2d& p_other) const noexcept
		{
			return m_pack.m_texture == p_other.m_pack.m_texture;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_texture2d<g_api_dx11>::operator!=(const bc_platform_texture2d& p_other) const noexcept
		{
			return !operator==(p_other);
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_texture2d<g_api_dx11>::operator==(std::nullptr_t) const noexcept
		{
			return !is_valid();
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_texture2d<g_api_dx11>::operator!=(std::nullptr_t) const noexcept
		{
			return is_valid();
		}
	}
}