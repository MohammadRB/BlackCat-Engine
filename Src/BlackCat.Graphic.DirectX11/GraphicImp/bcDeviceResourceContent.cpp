// [03/10/2017 MRB]

#include "GraphicImp/GraphicImpPCH.h"

#include "Core/bcConstant.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/bcDeviceResourceContent.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Shader/bcHullShader.h"
#include "GraphicImp/Shader/bcVertexShader.h"
#include "GraphicImp/Shader/bcPixelShader.h"
#include "GraphicImp/Shader/bcComputeShader.h"
#include "GraphicImp/Shader/bcDomainShader.h"
#include "GraphicImp/Shader/bcGeometryShader.h"

namespace black_cat
{
	namespace graphic
	{
		/*template<>
		BC_GRAPHICIMP_DLL
		constexpr bcCHAR* bc_device_resource_content< bc_texture2d >::content_name()
		{
			return BC_CONTENT_NAME(tex_2d);
		}

		template<>
		BC_GRAPHICIMP_DLL
		constexpr bcUINT32 bc_device_resource_content< bc_texture2d >::content_hash()
		{
			return BC_COMPILE_TIME_STRING_HASH(content_name());
		}

		template<>
		BC_GRAPHICIMP_DLL
		constexpr bcCHAR* bc_device_resource_content< bc_vertex_shader >::content_name()
		{
			return BC_CONTENT_NAME(vtx_shd);
		}

		template<>
		BC_GRAPHICIMP_DLL
		constexpr bcUINT32 bc_device_resource_content< bc_vertex_shader >::content_hash()
		{
			return BC_COMPILE_TIME_STRING_HASH(content_name());
		}

		template<>
		BC_GRAPHICIMP_DLL
		constexpr bcCHAR* bc_device_resource_content< bc_hull_shader >::content_name()
		{
			return BC_CONTENT_NAME(hul_shd);
		}

		template<>
		BC_GRAPHICIMP_DLL
		constexpr bcUINT32 bc_device_resource_content< bc_hull_shader >::content_hash()
		{
			return BC_COMPILE_TIME_STRING_HASH(content_name());
		}

		template<>
		BC_GRAPHICIMP_DLL
		constexpr bcCHAR* bc_device_resource_content< bc_domain_shader >::content_name()
		{
			return BC_CONTENT_NAME(dmn_shd);
		}

		template<>
		BC_GRAPHICIMP_DLL
		constexpr bcUINT32 bc_device_resource_content< bc_domain_shader >::content_hash()
		{
			return BC_COMPILE_TIME_STRING_HASH(content_name());
		}

		template<>
		BC_GRAPHICIMP_DLL
		constexpr bcCHAR* bc_device_resource_content< bc_geometry_shader >::content_name()
		{
			return BC_CONTENT_NAME(gmt_shd);
		}

		template<>
		BC_GRAPHICIMP_DLL
		constexpr bcUINT32 bc_device_resource_content< bc_geometry_shader >::content_hash()
		{
			return BC_COMPILE_TIME_STRING_HASH(content_name());
		}

		template<>
		BC_GRAPHICIMP_DLL
		constexpr bcCHAR* bc_device_resource_content< bc_pixel_shader >::content_name()
		{
			return BC_CONTENT_NAME(pxl_shd);
		}

		template<>
		BC_GRAPHICIMP_DLL
		constexpr bcUINT32 bc_device_resource_content< bc_pixel_shader >::content_hash()
		{
			return BC_COMPILE_TIME_STRING_HASH(content_name());
		}

		template<>
		BC_GRAPHICIMP_DLL
		constexpr bcCHAR* bc_device_resource_content< bc_compute_shader >::content_name()
		{
			return BC_CONTENT_NAME(cmp_shd);
		}

		template<>
		BC_GRAPHICIMP_DLL
		constexpr bcUINT32 bc_device_resource_content< bc_compute_shader >::content_hash()
		{
			return BC_COMPILE_TIME_STRING_HASH(content_name());
		}*/
	}
}