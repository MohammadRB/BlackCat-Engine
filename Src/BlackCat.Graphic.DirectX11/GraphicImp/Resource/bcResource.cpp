// [01/29/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Resource/bcResource.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bci_platform_resource< g_api_dx11 >::bci_platform_resource()
			: bc_platform_device_reference()
		{
			m_pack.m_resource = nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bci_platform_resource<g_api_dx11>::bci_platform_resource(platform_pack& p_pack)
			: bc_platform_device_reference()
		{
			m_pack.m_resource = p_pack.m_resource;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bci_platform_resource< g_api_dx11 >::~bci_platform_resource()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bci_platform_resource< g_api_dx11 >::bci_platform_resource(const bci_platform_resource& p_other)
			: bc_platform_device_reference(p_other)
		{
			m_pack.m_resource = p_other.m_pack.m_resource;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bci_platform_resource< g_api_dx11 >& bci_platform_resource< g_api_dx11 >::operator=(const bci_platform_resource& p_other)
		{
			bc_platform_device_reference::operator=(p_other);
			m_pack.m_resource = p_other.m_pack.m_resource;

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_resource_eviction_priority bci_platform_resource< g_api_dx11 >::get_eviction_priority() const
		{
			return static_cast< bc_resource_eviction_priority >(m_pack.m_resource->GetEvictionPriority());
		};

		template<>
		BC_GRAPHICIMP_DLL
		void bci_platform_resource< g_api_dx11 >::set_eviction_priority(bc_resource_eviction_priority p_eviction_priority)
		{
			m_pack.m_resource->SetEvictionPriority(static_cast< bcUINT >(p_eviction_priority));
		};
	}
}
