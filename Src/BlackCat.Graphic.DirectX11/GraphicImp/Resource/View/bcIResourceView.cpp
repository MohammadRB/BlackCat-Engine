// [03/09/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Resource/View/bcIResourceView.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL 
		bc_platform_iresource_view< g_api_dx11 >::bc_platform_iresource_view()
			: bc_platform_device_reference()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_iresource_view<g_api_dx11>::bc_platform_iresource_view(platform_pack& p_pack)
			: bc_platform_device_reference()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL bc_platform_iresource_view< g_api_dx11 >::bc_platform_iresource_view(const bc_platform_iresource_view& p_other)
			: bc_platform_device_reference(p_other)
		{
		}

		template<>
		BC_GRAPHICIMP_DLL bc_platform_iresource_view< g_api_dx11 >::~bc_platform_iresource_view()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL bc_platform_iresource_view< g_api_dx11 >& bc_platform_iresource_view< g_api_dx11 >::operator=(const bc_platform_iresource_view& p_other)
		{
			bc_platform_device_reference::operator=(std::move(p_other));

			return *this;
		}
	}
}