// [01/27/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"

#include "Core/Container/bcAllocator.h"
#include "CorePlatformImp/bcUtility.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Shader/bcShader.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_compiled_shader<g_api_dx11>::is_valid() const noexcept
		{
			return get_platform_pack().m_blob != nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_compiled_shader<g_api_dx11>::set_debug_name(const bcCHAR* p_name) noexcept
		{
		}
	}
}