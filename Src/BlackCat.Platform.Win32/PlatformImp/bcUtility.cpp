// [25/05/2021 MRB]

#include "PlatformImp/PlatformImpPCH.h"

#include "Core/Container/bcString.h"
#include "Platform/bcException.h"
#include "PlatformImp/bcUtility.h"
#include <WinSock2.h>

namespace black_cat::platform
{
	void bc_throw_network_exception()
	{
		bcCHAR l_error_string[MAX_PATH];
		const auto l_error_code = WSAGetLastError();

		FormatMessageA
		(
			FORMAT_MESSAGE_FROM_SYSTEM,
			0,
			l_error_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			l_error_string,
			MAX_PATH,
			NULL
		);

		core::bc_string_frame l_message = "Win32 socket error: ";
		l_message += l_error_string;

		throw bc_network_exception(l_error_code, l_message.c_str());
	}

	void bc_throw_network_exception(const bcCHAR* p_message)
	{
		core::bc_string_frame l_message = "Win32 socket error: ";
		l_message += p_message;

		throw bc_network_exception(l_message.c_str());
	}

	bcINT bc_cast_from_address_family(bc_socket_address p_address_family)
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

	bc_socket_address bc_cast_to_address_family(bcINT p_address_family)
	{
		switch (p_address_family)
		{
		case AF_INET:
			return bc_socket_address::inter_network;
		case AF_INET6:
			return bc_socket_address::inter_network_v6;
		default:
			BC_ASSERT(false);
			return static_cast<bc_socket_address>(0);
		}
	}

	bcINT bc_cast_from_socket_type(bc_socket_type p_socket_type)
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

	bc_socket_type bc_cast_to_socket_type(bcINT p_socket_type)
	{
		switch (p_socket_type)
		{
		case SOCK_DGRAM:
			return bc_socket_type::data_gram;
		case SOCK_STREAM:
			return bc_socket_type::stream;
		default:
			BC_ASSERT(false);
			return static_cast<bc_socket_type>(0);
		}
	}

	bcINT bc_cast_from_socket_protocol(bc_socket_protocol p_socket_protocol)
	{
		switch (p_socket_protocol)
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

	bc_socket_protocol bc_cast_to_socket_protocol(bcINT p_socket_protocol)
	{
		switch (p_socket_protocol)
		{
		case IPPROTO_TCP:
			return bc_socket_protocol::tcp;
		case IPPROTO_UDP:
			return bc_socket_protocol::udp;
		default:
			BC_ASSERT(false);
			return static_cast<bc_socket_protocol>(0);
		}
	}
}
