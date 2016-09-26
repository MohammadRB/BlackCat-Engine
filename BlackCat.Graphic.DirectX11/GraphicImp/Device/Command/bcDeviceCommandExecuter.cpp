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
		bc_platform_device_command_executer< bc_platform_render_api::directx11 >::bc_platform_device_command_executer(bc_device& p_device)
			: m_device(p_device), 
			m_pack()
		{
		};

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_executer< bc_platform_render_api::directx11 >::bc_platform_device_command_executer(bc_platform_device_command_executer&& p_other)
			: bc_device_object(std::move(p_other)),
			m_device(p_other.m_device), 
			m_pack(std::move(p_other.m_pack))
		{
		};

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_executer< bc_platform_render_api::directx11 >::~bc_platform_device_command_executer()
		{
		};

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_command_executer< bc_platform_render_api::directx11 >& bc_platform_device_command_executer< bc_platform_render_api::directx11 >::operator=(bc_platform_device_command_executer&& p_other)
		{
			bc_device_object::operator=(std::move(p_other));
			m_pack = std::move(p_other.m_pack);

			return *this;
		};

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_command_executer< bc_platform_render_api::directx11 >::excecute_command_list(bc_device_command_list* p_command_list)
		{
			m_device.get_platform_pack().m_immediate_context->ExecuteCommandList(p_command_list->get_platform_pack().m_command_list.Get(), false);
		};
	}
}
