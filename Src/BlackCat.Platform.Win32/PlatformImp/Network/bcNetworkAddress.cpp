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

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_network_address<core_platform::g_api_win32>::bc_platform_network_address(platform_pack& p_pack)
			: m_pack(p_pack)
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_network_address<core_platform::g_api_win32>::bc_platform_network_address(const bc_platform_network_address<core_platform::g_api_win32>& p_other) noexcept
			: m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_network_address<core_platform::g_api_win32>::~bc_platform_network_address() = default;

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_network_address<core_platform::g_api_win32>& bc_platform_network_address<core_platform::g_api_win32>::operator=(const bc_platform_network_address<core_platform::g_api_win32>& p_other) noexcept
		{
			m_pack = p_other.m_pack;
			return *this;
		}

		template<>
		BC_PLATFORMIMP_DLL
		std::tuple<bc_socket_address, core::bc_string_frame, bcUINT16> bc_platform_network_address<core_platform::g_api_win32>::get_traits() const noexcept
		{
			char l_ip[INET_ADDRSTRLEN];

			inet_ntop(m_pack.m_address.sin_family, &m_pack.m_address.sin_addr, l_ip, sizeof(l_ip));
			bc_socket_address l_family = bc_cast_to_address_family(m_pack.m_address.sin_family);
			bcUINT16 l_port = ntohs(m_pack.m_address.sin_port);

			return std::make_tuple(l_family, core::bc_string_frame(&l_ip[0]), l_port);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_network_address<core_platform::g_api_win32>::is_valid() const noexcept
		{
			return m_pack.m_address.sin_port != 0;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_network_address<core_platform::g_api_win32>::operator==(const bc_platform_network_address<core_platform::g_api_win32>& p_other) const noexcept
		{
			return m_pack.m_address.sin_family == p_other.m_pack.m_address.sin_family &&
					m_pack.m_address.sin_port == p_other.m_pack.m_address.sin_port &&
					m_pack.m_address.sin_addr.s_addr == p_other.m_pack.m_address.sin_addr.s_addr;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_network_address<core_platform::g_api_win32>::operator!=(const bc_platform_network_address<core_platform::g_api_win32>& p_other) const noexcept
		{
			return !(*this == p_other);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_network_address<core_platform::g_api_win32> bc_platform_network_address<core_platform::g_api_win32>::from_ip_port(bc_socket_address p_address_family, const bcCHAR* p_ip, bcUINT16 p_port) noexcept
		{
			platform_pack l_pack;

			l_pack.m_address.sin_family = bc_cast_from_address_family(p_address_family);
			l_pack.m_address.sin_port = htons(p_port);
			
			const auto l_ip_convert = inet_pton(l_pack.m_address.sin_family, p_ip, &l_pack.m_address.sin_addr.s_addr);
			if (l_ip_convert == 0 || l_ip_convert == -1)
			{
				ZeroMemory(&l_pack.m_address, sizeof(sockaddr_in));
			}
			/*if(l_ip_convert == 0)
			{
				bc_throw_network_exception("Invalid ip address");
			}
			else if(l_ip_convert == -1)
			{
				bc_throw_network_exception();
			}*/
			
			return bc_network_address(l_pack);
		}
	}
}