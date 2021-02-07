// [02/11/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Resource/bcResource.h"
#include "GraphicImp/Resource/View/bcResourceViewConfig.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Device/bcDevice.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_resource_view<g_api_dx11>::bc_platform_resource_view()
			: bci_platform_resource_view(),
			m_type(bc_resource_view_type::shader)
		{
			m_pack.m_shader_view = nullptr;
			m_pack.m_unordered_shader_view = nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_resource_view< g_api_dx11 >::bc_platform_resource_view(platform_pack& p_pack, bc_resource_view_type p_type)
			: bci_platform_resource_view(),
			m_type(p_type)
		{
			m_pack.m_shader_view = p_pack.m_shader_view;
			m_pack.m_unordered_shader_view = p_pack.m_unordered_shader_view;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_resource_view< g_api_dx11 >::~bc_platform_resource_view()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_resource_view< g_api_dx11 >::bc_platform_resource_view(const bc_platform_resource_view& p_other)
			: bci_platform_resource_view(p_other),
			m_type(p_other.m_type)
		{
			m_pack.m_shader_view = p_other.m_pack.m_shader_view;
			m_pack.m_unordered_shader_view = p_other.m_pack.m_unordered_shader_view;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_resource_view< g_api_dx11 >& bc_platform_resource_view< g_api_dx11 >::operator=(const bc_platform_resource_view& p_other)
		{
			bci_platform_resource_view::operator=(p_other);
			m_type = p_other.m_type;
			m_pack.m_shader_view = p_other.m_pack.m_shader_view;
			m_pack.m_unordered_shader_view = p_other.m_pack.m_unordered_shader_view;

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_resource_view_type bc_platform_resource_view< g_api_dx11 >::get_view_type() const
		{
			return m_type;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_resource_view< g_api_dx11 >::is_valid() const noexcept
		{
			return m_pack.m_shader_view != nullptr || m_pack.m_unordered_shader_view != nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_resource_view<g_api_dx11>::set_debug_name(const bcCHAR* p_name) noexcept
		{
			if (is_valid())
			{
				if(m_pack.m_shader_view)
				{
					m_pack.m_shader_view->SetPrivateData(WKPDID_D3DDebugObjectName, std::strlen(p_name), p_name);
				}
				if(m_pack.m_unordered_shader_view)
				{
					m_pack.m_unordered_shader_view->SetPrivateData(WKPDID_D3DDebugObjectName, std::strlen(p_name), p_name);
				}
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
			bool bc_platform_resource_view<g_api_dx11>::operator==(const bc_platform_resource_view& p_other) const noexcept
		{
			if (m_type != p_other.m_type)
			{
				return false;
			}

			if (m_type == bc_resource_view_type::shader)
			{
				return m_pack.m_shader_view == p_other.m_pack.m_shader_view;
			}

			if (m_type == bc_resource_view_type::unordered)
			{
				return m_pack.m_unordered_shader_view == p_other.m_pack.m_unordered_shader_view;
			}

			BC_ASSERT(false);
			return false;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_resource_view<g_api_dx11>::operator!=(const bc_platform_resource_view& p_other) const noexcept
		{
			return !operator==(p_other);
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_resource_view<g_api_dx11>::operator==(std::nullptr_t) const noexcept
		{
			return !is_valid();
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_resource_view<g_api_dx11>::operator!=(std::nullptr_t) const noexcept
		{
			return is_valid();
		}
	}
}
