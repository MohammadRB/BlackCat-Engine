// [01/27/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		BC_GRAPHICIMP_DLL_EXP 
		bc_platform_device_command_list<bc_platform_render_api::directx11>::bc_platform_device_command_list()
			: m_pack()
		{
		};

		template< >
		BC_GRAPHICIMP_DLL_EXP 
		bc_platform_device_command_list<bc_platform_render_api::directx11>::bc_platform_device_command_list(bc_platform_device_command_list&& p_other)
			: m_pack(std::move(p_other.m_pack))
		{
		};

		template< >
		BC_GRAPHICIMP_DLL_EXP 
		bc_platform_device_command_list<bc_platform_render_api::directx11>::~bc_platform_device_command_list()
		{
		};

		template< >
		BC_GRAPHICIMP_DLL_EXP 
		bc_platform_device_command_list<bc_platform_render_api::directx11>& bc_platform_device_command_list<bc_platform_render_api::directx11>::operator=(bc_platform_device_command_list&& p_other)
		{
			m_pack = std::move(p_other.m_pack);

			return *this;
		};

		template<>
		BC_GRAPHICIMP_DLL_EXP
		void bc_platform_device_command_list< bc_platform_render_api::directx11 >::reset()
		{
			m_pack.m_command_list.ReleaseAndGetAddressOf();
		}
	}
}