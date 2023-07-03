// [27/01/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecutor.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_executor<g_api_dx11>::bc_platform_device_command_executor() noexcept
			: bc_platform_device_reference()
		{
			m_pack.m_device = nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_executor<g_api_dx11>::bc_platform_device_command_executor(platform_pack& p_pack) noexcept
			: bc_platform_device_reference(p_pack),
			m_pack(p_pack)
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_executor<g_api_dx11>::bc_platform_device_command_executor(const bc_platform_device_command_executor& p_other) noexcept
			: bc_platform_device_reference(p_other),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_executor<g_api_dx11>::~bc_platform_device_command_executor() = default;

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_executor<g_api_dx11>& bc_platform_device_command_executor<g_api_dx11>::operator=(const bc_platform_device_command_executor& p_other) noexcept
		{
			bc_platform_device_reference::operator=(p_other);
			m_pack = p_other.m_pack;

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_command_executor<g_api_dx11>::execute_command_list(bc_device_command_list p_command_list)
		{
			{
				platform::bc_mutex_guard l_lock(m_pack.m_device->get_platform_pack().m_immediate_context_mutex);

				m_pack.m_device->get_platform_pack().m_immediate_context->ExecuteCommandList
				(
					p_command_list.get_platform_pack().m_command_list_proxy->m_command_list, 
					false
				);
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_command_executor<g_api_dx11>::is_valid() const noexcept
		{
			return true;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_command_executor<g_api_dx11>::set_debug_name(const bcCHAR* p_name) noexcept
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_command_executor<g_api_dx11>::operator==(const bc_platform_device_command_executor& p_other) const noexcept
		{
			return true;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_command_executor<g_api_dx11>::operator!=(const bc_platform_device_command_executor& p_other) const noexcept
		{
			return !operator==(p_other);
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_command_executor<g_api_dx11>::operator==(std::nullptr_t) const noexcept
		{
			return !is_valid();
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_command_executor<g_api_dx11>::operator!=(std::nullptr_t) const noexcept
		{
			return is_valid();
		}
	}
}