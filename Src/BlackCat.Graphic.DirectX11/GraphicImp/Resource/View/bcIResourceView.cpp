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
		bci_platform_resource_view< g_api_dx11 >::bci_platform_resource_view()
			: bc_platform_device_reference()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bci_platform_resource_view<g_api_dx11>::bci_platform_resource_view(platform_pack& p_pack)
			: bc_platform_device_reference()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL bci_platform_resource_view< g_api_dx11 >::bci_platform_resource_view(const bci_platform_resource_view& p_other)
			: bc_platform_device_reference(p_other)
		{
		}

		template<>
		BC_GRAPHICIMP_DLL bci_platform_resource_view< g_api_dx11 >::~bci_platform_resource_view()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL bci_platform_resource_view< g_api_dx11 >& bci_platform_resource_view< g_api_dx11 >::operator=(const bci_platform_resource_view& p_other)
		{
			bc_platform_device_reference::operator=(std::move(p_other));

			return *this;
		}
	}
}