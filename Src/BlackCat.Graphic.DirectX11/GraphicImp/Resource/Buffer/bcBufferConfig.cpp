// [03/17/2017 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Resource/Buffer/bcBufferConfig.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		bcUINT bc_platform_buffer_config<g_api_dx11>::get_byte_width() const
		{
			return m_pack.m_desc.ByteWidth;
		}

		template< >
		bc_resource_usage bc_platform_buffer_config<g_api_dx11>::get_usage() const
		{
			return bc_graphic_cast(m_pack.m_desc.Usage);
		}

		template< >
		bcUINT bc_platform_buffer_config<g_api_dx11>::get_structure_byte_stride() const
		{
			return m_pack.m_desc.StructureByteStride;
		}
	}
}