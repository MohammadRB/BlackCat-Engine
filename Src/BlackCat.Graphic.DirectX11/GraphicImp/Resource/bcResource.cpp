// [29/01/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Resource/bcResource.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_resource_eviction_priority bci_platform_resource< g_api_dx11 >::get_eviction_priority() const
		{
			return static_cast<bc_resource_eviction_priority>(get_resource_platform_pack().m_resource->GetEvictionPriority());
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bci_platform_resource< g_api_dx11 >::set_eviction_priority(bc_resource_eviction_priority p_eviction_priority)
		{
			get_resource_platform_pack().m_resource->SetEvictionPriority(static_cast< bcUINT >(p_eviction_priority));
		}
	}
}