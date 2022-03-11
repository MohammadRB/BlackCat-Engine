// [05/31/2021 MRB]

#pragma once

#include "PlatformImp/Network/bcNetworkDefinitions.h"
#include "PlatformImp/Network/bcNetworkAddress.h"

#include "Platform/Network/bcNonBlockSocket.h"
#include <WinSock2.h>

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_non_block_socket_pack<platform::g_api_win32>
		{
			SOCKET m_socket;
		};
	}
}