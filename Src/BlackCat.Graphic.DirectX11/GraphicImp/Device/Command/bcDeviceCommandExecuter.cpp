// [01/27/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecuter.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_executer<g_api_dx11>::bc_platform_device_command_executer()
		{
			m_pack.m_device = nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_executer< g_api_dx11 >::bc_platform_device_command_executer(platform_pack& p_pack)
		{
			m_pack.m_device = p_pack.m_device;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_executer< g_api_dx11 >::bc_platform_device_command_executer(const bc_platform_device_command_executer& p_other)
			: bc_platform_device_reference(std::move(p_other))
		{
			m_pack.m_device = p_other.m_pack.m_device;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_executer< g_api_dx11 >::~bc_platform_device_command_executer()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_executer< g_api_dx11 >& bc_platform_device_command_executer< g_api_dx11 >::operator=(const bc_platform_device_command_executer& p_other)
		{
			bc_platform_device_reference::operator=(p_other);
			m_pack.m_device = p_other.m_pack.m_device;

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_command_executer< g_api_dx11 >::excecute_command_list(bc_device_command_list p_command_list)
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> l_guard(m_pack.m_device->get_platform_pack().m_context_mutex);

				m_pack.m_device->get_platform_pack().m_immediate_context->ExecuteCommandList
				(
					p_command_list.get_platform_pack().m_command_list_proxy->m_command_list, 
					false
				);
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_command_executer<g_api_dx11>::is_valid() const noexcept
		{
			return true;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_command_executer<g_api_dx11>::operator==(const bc_platform_device_command_executer& p_other) const noexcept
		{
			return true;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_command_executer<g_api_dx11>::operator!=(const bc_platform_device_command_executer& p_other) const noexcept
		{
			return !operator==(p_other);
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_command_executer<g_api_dx11>::operator==(std::nullptr_t) const noexcept
		{
			return !is_valid();
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_command_executer<g_api_dx11>::operator!=(std::nullptr_t) const noexcept
		{
			return is_valid();
		}
	}
}