// [05/25/2021 MRB]

#pragma once

#include "PlatformImp/Network/bcNetworkDefinitions.h"

#include "Platform/Network/bcNetworkAddress.h"
#include <WinSock2.h>

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_network_address_pack<core_platform::g_api_win32>
		{
			sockaddr_in m_address;
		};
	}	
}