// [01/30/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <ScreenGrab.h>
#include <wincodec.h>
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
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_texture2d< g_api_dx11 >::bc_platform_texture2d()
			: m_pack()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_texture2d< g_api_dx11 >::~bc_platform_texture2d()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_texture2d< g_api_dx11 >::bc_platform_texture2d(bc_platform_texture2d&& p_other)
			: bc_iresource(std::move(p_other)),
			m_pack(std::move(p_other.m_pack))
		{
		}

		template<>
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_texture2d< g_api_dx11 >& bc_platform_texture2d< g_api_dx11 >::operator=(bc_platform_texture2d&& p_other)
		{
			bc_iresource::operator=(std::move(p_other));
			m_pack = std::move(p_other.m_pack);

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL_EXP
		bcUINT bc_platform_texture2d< g_api_dx11 >::get_width() const
		{
			return const_cast< bc_platform_texture2d* >(this)->m_pack.m_config.get_platform_pack().m_desc.Width;
		}

		template<>
		BC_GRAPHICIMP_DLL_EXP
		bcUINT bc_platform_texture2d< g_api_dx11 >::get_height() const
		{
			return const_cast< bc_platform_texture2d* >(this)->m_pack.m_config.get_platform_pack().m_desc.Height;
		}

		template<>
		BC_GRAPHICIMP_DLL_EXP
		bc_resource_usage bc_platform_texture2d< g_api_dx11 >::get_usage() const
		{
			return bc_graphic_cast(const_cast< bc_platform_texture2d* >(this)->m_pack.m_config.get_platform_pack().m_desc.Usage);
		}

		template<>
		BC_GRAPHICIMP_DLL_EXP
		bool bc_platform_texture2d< g_api_dx11 >::is_multisampled() const
		{
			return const_cast< bc_platform_texture2d* >(this)->m_pack.m_config.get_platform_pack().m_desc.SampleDesc.Count > 1;
		}

		template<>
		BC_GRAPHICIMP_DLL_EXP
		bc_texture_ms_config bc_platform_texture2d< g_api_dx11 >::get_sample_count() const
		{
			auto& l_sample_desc = const_cast<bc_platform_texture2d*>(this)->m_pack.m_config.get_platform_pack().m_desc.SampleDesc;
			return bc_texture_ms_config(l_sample_desc.Count, l_sample_desc.Quality);
		}

		template<>
		BC_GRAPHICIMP_DLL_EXP
		bc_format bc_platform_texture2d< g_api_dx11 >::get_format() const
		{
			return bc_graphic_cast(const_cast< bc_platform_texture2d* >(this)->m_pack.m_config.get_platform_pack().m_desc.Format);
		}
	}
}
