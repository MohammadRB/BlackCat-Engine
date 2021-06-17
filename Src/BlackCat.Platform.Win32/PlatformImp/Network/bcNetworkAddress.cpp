// [05/25/2021 MRB]

#include "PlatformImp/PlatformImpPCH.h"

#include "Platform/bcException.h"
#include "PlatformImp/Network/bcNetworkAddress.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/bcUtility.h"
#include <WS2tcpip.h>

namespace black_cat
{
	namespace platform
	{
		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_network_address<core_platform::g_api_win32>::bc_platform_network_address()
		{
			ZeroMemory(&m_pack.m_address, sizeof(sockaddr_in));
		}

		template<core_platform::bc_platform TPlatform>
		bc_platform_network_address<TPlatform>::bc_platform_network_address(platform_pack& p_pack)
			: m_pack(p_pack)
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_network_address<core_platform::g_api_win32>::bc_platform_network_address(const bc_platform_network_address& p_other) noexcept = default;

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_network_address<core_platform::g_api_win32>::~bc_platform_network_address() = default;

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_network_address<core_platform::g_api_win32>& bc_platform_network_address<core_platform::g_api_win32>::operator=(const bc_platform_network_address& p_other) noexcept = default;

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_network_address<core_platform::g_api_win32> bc_platform_network_address<core_platform::g_api_win32>::from_ip_port(bc_socket_address p_address_family, const bcCHAR* p_ip, bcUINT32 p_port)
		{
			platform_pack l_pack;

			l_pack.m_address.sin_port = p_port;
			l_pack.m_address.sin_family = bc_cast_from_address_family(p_address_family);
			
			const auto l_ip_convert = inet_pton(l_pack.m_address.sin_family, p_ip, &l_pack.m_address.sin_addr.S_un.S_addr);
			if(l_ip_convert == 0)
			{
				bc_throw_network_exception("Invalid ip address");
			}
			else if(l_ip_convert == -1)
			{
				bc_throw_network_exception();
			}
			
			return bc_network_address(l_pack);
		}
	}
}