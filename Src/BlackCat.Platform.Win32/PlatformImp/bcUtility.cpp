// [05/25/2021 MRB]

#include "PlatformImp/PlatformImpPCH.h"

#include "Core/Container/bcString.h"
#include "Platform/bcException.h"
#include "PlatformImp/bcUtility.h"
#include <WinSock2.h>

namespace black_cat
{
	namespace platform
	{
		void bc_throw_network_exception()
		{
			bcCHAR l_error_string[MAX_PATH];

			FormatMessageA
			(
				FORMAT_MESSAGE_FROM_SYSTEM,
				0,
				WSAGetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				l_error_string,
				MAX_PATH,
				NULL
			);

			bc_throw_network_exception(l_error_string);
		}

		void bc_throw_network_exception(const bcCHAR* p_message)
		{
			core::bc_string_frame l_message = "Win32 socket error: ";
			l_message += p_message;

			throw bc_network_exception(l_message.c_str());
		}

		bcINT bc_platform_cast(bc_socket_address p_address_family)
		{
			switch (p_address_family)
			{
			case bc_socket_address::inter_network:
				return AF_INET;
			case bc_socket_address::inter_network_v6:
				return AF_INET6;
			default:
				BC_ASSERT(false);
				return 0;
			}
		}

		bcINT bc_platform_cast(bc_socket_type p_socket_type)
		{
			switch (p_socket_type)
			{
			case bc_socket_type::data_gram:
				return SOCK_DGRAM;
			case bc_socket_type::stream:
				return SOCK_STREAM;
			default: 
				BC_ASSERT(false);
				return 0;
			}
		}

		bcINT bc_platform_cast(bc_socket_protocol p_socket_type)
		{
			switch (p_socket_type)
			{
			case bc_socket_protocol::tcp:
				return IPPROTO_TCP;
			case bc_socket_protocol::udp:
				return IPPROTO_UDP;
			default:
				BC_ASSERT(false);
				return 0;
			}
		}
	}
}