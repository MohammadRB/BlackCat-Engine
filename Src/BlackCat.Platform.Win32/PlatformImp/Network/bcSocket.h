// [05/25/2021 MRB]

#pragma once

#include "PlatformImp/Network/bcNetworkDefinitions.h"
#include "PlatformImp/Network/bcNetworkAddress.h"

#include "Platform/Network/bcSocket.h"
#include <WinSock2.h>

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_socket_pack<core_platform::g_api_win32>
		{
			SOCKET m_socket;
		};
	}	
}