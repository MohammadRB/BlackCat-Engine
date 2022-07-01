// [01/27/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"

namespace black_cat
{
	namespace graphic
	{
		// provide definitions at first to prevent CLang 'explicit specialization of '' after instantiation' error

		template<>
		bool bc_platform_device_command_list<g_api_dx11>::is_valid() const noexcept;

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_list<g_api_dx11>::bc_platform_device_command_list() noexcept
		{
			m_pack.m_command_list_proxy = nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_list<g_api_dx11>::bc_platform_device_command_list(platform_pack& p_pack) noexcept
			: bc_platform_device_reference(p_pack),
			m_pack(p_pack)
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_list<g_api_dx11>::bc_platform_device_command_list(const bc_platform_device_command_list& p_other) noexcept
			: bc_platform_device_reference(p_other),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_list<g_api_dx11>::~bc_platform_device_command_list()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_list<g_api_dx11>& bc_platform_device_command_list<g_api_dx11>::operator=(const bc_platform_device_command_list& p_other) noexcept
		{
			bc_platform_device_reference::operator=(p_other);
			m_pack = p_other.m_pack;

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_command_list<g_api_dx11>::finished()
		{
			if(is_valid() && m_pack.m_command_list_proxy->m_command_list)
			{
				m_pack.m_command_list_proxy->m_command_list->Release();
				m_pack.m_command_list_proxy->m_command_list = nullptr;
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_command_list<g_api_dx11>::is_valid() const noexcept
		{
			return m_pack.m_command_list_proxy != nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_command_list<g_api_dx11>::set_debug_name(const bcCHAR* p_name) noexcept
		{
			if (is_valid())
			{
				m_pack.m_command_list_proxy->m_command_list->SetPrivateData(WKPDID_D3DDebugObjectName, std::strlen(p_name), p_name);
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_command_list<g_api_dx11>::operator==(const bc_platform_device_command_list& p_other) const noexcept
		{
			return m_pack.m_command_list_proxy == p_other.m_pack.m_command_list_proxy;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_command_list<g_api_dx11>::operator!=(const bc_platform_device_command_list& p_other) const noexcept
		{
			return !operator==(p_other);
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_command_list<g_api_dx11>::operator==(std::nullptr_t) const noexcept
		{
			return !is_valid();
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_command_list<g_api_dx11>::operator!=(std::nullptr_t) const noexcept
		{
			return is_valid();
		}
	}
}