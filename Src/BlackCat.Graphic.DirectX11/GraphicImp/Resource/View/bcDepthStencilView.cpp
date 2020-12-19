// [02/11/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Resource/bcResource.h"
#include "GraphicImp/Resource/View/bcDepthStencilViewConfig.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"
#include "GraphicImp/Device/bcDevice.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_depth_stencil_view< g_api_dx11 >::bc_platform_depth_stencil_view()
			: bc_platform_iresource_view()
		{
			m_pack.m_depth_stencil_view = nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_depth_stencil_view<g_api_dx11>::bc_platform_depth_stencil_view(platform_pack& p_pack)
			: bc_platform_iresource_view()
		{
			m_pack.m_depth_stencil_view = p_pack.m_depth_stencil_view;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_depth_stencil_view< g_api_dx11 >::~bc_platform_depth_stencil_view()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_depth_stencil_view< g_api_dx11 >::bc_platform_depth_stencil_view(const bc_platform_depth_stencil_view& p_other)
			: bc_platform_iresource_view(p_other)
		{
			m_pack.m_depth_stencil_view = p_other.m_pack.m_depth_stencil_view;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_depth_stencil_view< g_api_dx11 >& bc_platform_depth_stencil_view< g_api_dx11 >::operator=(const bc_platform_depth_stencil_view& p_other)
		{
			bc_platform_iresource_view::operator=(p_other);
			m_pack.m_depth_stencil_view = p_other.m_pack.m_depth_stencil_view;

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_resource_view_type bc_platform_depth_stencil_view< g_api_dx11 >::get_view_type() const
		{
			return bc_resource_view_type::depth_stencil;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_depth_stencil_view<g_api_dx11>::is_valid() const noexcept
		{
			return m_pack.m_depth_stencil_view != nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_depth_stencil_view<g_api_dx11>::set_debug_name(const bcCHAR* p_name) noexcept
		{
			if (is_valid())
			{
				m_pack.m_depth_stencil_view->SetPrivateData(WKPDID_D3DDebugObjectName, std::strlen(p_name), p_name);
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_depth_stencil_view<g_api_dx11>::operator==(const bc_platform_depth_stencil_view& p_other) const noexcept
		{
			return m_pack.m_depth_stencil_view == p_other.m_pack.m_depth_stencil_view;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_depth_stencil_view<g_api_dx11>::operator!=(const bc_platform_depth_stencil_view& p_other) const noexcept
		{
			return !operator==(p_other);
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_depth_stencil_view<g_api_dx11>::operator==(std::nullptr_t) const noexcept
		{
			return !is_valid();
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_depth_stencil_view<g_api_dx11>::operator!=(std::nullptr_t) const noexcept
		{
			return is_valid();
		}
	}
}