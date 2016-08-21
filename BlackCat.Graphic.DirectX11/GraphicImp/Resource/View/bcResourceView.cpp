// [03/09/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Resource/View/bcResourceView.h"

namespace black_cat
{
	namespace graphic
	{
		template < >
		BC_GRAPHICIMP_DLL bc_platform_iresource_view<g_api_dx11>::bc_platform_iresource_view()
			: m_pack()
		{
		}

		template < >
		BC_GRAPHICIMP_DLL bc_platform_iresource_view<g_api_dx11>::bc_platform_iresource_view(bc_platform_iresource_view&& p_other)
			: bc_device_object(std::move(p_other)),
			m_pack(std::move(p_other.m_pack))
		{
		}

		template < >
		BC_GRAPHICIMP_DLL bc_platform_iresource_view<g_api_dx11>::~bc_platform_iresource_view()
		{
		}

		template < >
		BC_GRAPHICIMP_DLL bc_platform_iresource_view<g_api_dx11>& bc_platform_iresource_view<g_api_dx11>::operator=(bc_platform_iresource_view&& p_other)
		{
			bc_device_object::operator=(std::move(p_other));
			m_pack = std::move(p_other.m_pack);

			return *this;
		}
	}
}
