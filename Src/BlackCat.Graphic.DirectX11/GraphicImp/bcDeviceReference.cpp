// [12/22/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/bcDeviceReference.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_reference<g_api_dx11>::~bc_platform_device_reference()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_reference<g_api_dx11>::bc_platform_device_reference()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_reference<g_api_dx11>::bc_platform_device_reference(platform_pack& p_pack)
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_reference<g_api_dx11>::bc_platform_device_reference(const bc_platform_device_reference&)
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_reference<g_api_dx11>& bc_platform_device_reference<g_api_dx11>::operator=(const bc_platform_device_reference&)
		{
			return *this;
		}
	}
}