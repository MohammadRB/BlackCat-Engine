// [01/29/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Resource/bcResource.h"
#include "GraphicImp/Resource/Buffer/bcBufferConfig.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Device/bcDevice.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		BC_GRAPHICIMP_DLL
		bc_platform_buffer<g_api_dx11>::bc_platform_buffer()
			: m_pack()
		{
		};

		template< >
		BC_GRAPHICIMP_DLL 
		bc_platform_buffer<g_api_dx11>::~bc_platform_buffer()
		{
		};

		template< >
		BC_GRAPHICIMP_DLL 
		bc_platform_buffer<g_api_dx11>::bc_platform_buffer(bc_platform_buffer&& p_other)
			: bc_iresource(std::move(p_other)),
			m_pack(std::move(p_other.m_pack))
		{
		};

		template< >
		BC_GRAPHICIMP_DLL 
		bc_platform_buffer<g_api_dx11>& bc_platform_buffer<g_api_dx11>::operator=(bc_platform_buffer&& p_other)
		{
			bc_iresource::operator=(std::move(p_other));
			m_pack = std::move(p_other.m_pack);

			return *this;
		};

		template<  >
		BC_GRAPHICIMP_DLL
		bcUINT bc_platform_buffer<g_api_dx11>::get_byte_width() const
		{
			return const_cast< bc_platform_buffer* >(this)->m_pack.m_config.get_platform_pack().m_desc.ByteWidth;
		}

		template<  >
		BC_GRAPHICIMP_DLL
		bc_resource_usage bc_platform_buffer<g_api_dx11>::get_usage() const
		{
			return bc_graphic_cast(const_cast< bc_platform_buffer* >(this)->m_pack.m_config.get_platform_pack().m_desc.Usage);
		}

		template<  >
		BC_GRAPHICIMP_DLL
		bcUINT bc_platform_buffer<g_api_dx11>::get_structure_byte_stride() const
		{
			return const_cast< bc_platform_buffer* >(this)->m_pack.m_config.get_platform_pack().m_desc.StructureByteStride;
		}
	}
}