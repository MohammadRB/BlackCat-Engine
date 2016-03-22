// [02/11/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/Resource/bcResource.h"
#include "GraphicImp/Resource/View/bcResourceViewConfig.h"
#include "GraphicImp/Resource/View/bcShaderView.h"
#include "GraphicImp/Device/bcDevice.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_shader_view< g_api_dx11 >::bc_platform_shader_view()
			: m_pack()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_shader_view< g_api_dx11 >::~bc_platform_shader_view()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_shader_view< g_api_dx11 >::bc_platform_shader_view(bc_platform_shader_view&& p_other)
			: bc_iresource_view(std::move(p_other)),
			m_pack(std::move(p_other.m_pack))
		{
		}

		template<>
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_shader_view< g_api_dx11 >& bc_platform_shader_view< g_api_dx11 >::operator=(bc_platform_shader_view&& p_other)
		{
			bc_iresource_view::operator=(std::move(p_other));
			m_pack = std::move(p_other.m_pack);

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL_EXP
		bc_resource_view_type bc_platform_shader_view< g_api_dx11 >::get_view_type() const
		{
			return m_pack.m_type;
		}
	}
}
