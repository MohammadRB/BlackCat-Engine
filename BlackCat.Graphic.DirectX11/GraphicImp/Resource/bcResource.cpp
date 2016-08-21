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
		bc_platform_iresource< g_api_dx11 >::bc_platform_iresource()
			: m_pack()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_iresource< g_api_dx11 >::~bc_platform_iresource()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_iresource< g_api_dx11 >::bc_platform_iresource(bc_platform_iresource&& p_other)
			: bc_device_object(std::move(p_other)),
			m_pack(std::move(p_other.m_pack))
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_iresource< g_api_dx11 >& bc_platform_iresource< g_api_dx11 >::operator=(bc_platform_iresource&& p_other)
		{
			bc_device_object::operator=(std::move(p_other));
			m_pack = std::move(p_other.m_pack);

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_resource_eviction_priority bc_platform_iresource< g_api_dx11 >::get_eviction_priority() const
		{
			return static_cast< bc_resource_eviction_priority >(m_pack.m_resource->GetEvictionPriority());
		};

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_iresource< g_api_dx11 >::set_eviction_priority(bc_resource_eviction_priority p_eviction_priority)
		{
			m_pack.m_resource->SetEvictionPriority(static_cast< bcUINT >(p_eviction_priority));
		};
	}
}
