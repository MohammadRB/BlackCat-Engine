// [25/05/2021 MRB]

#pragma once

#include "PlatformImp/Network/bcNetworkDefinitions.h"

#include <WinSock2.h>
#include "Platform/Network/bcNetworkAddress.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_network_address_pack<platform::g_api_win32>
		{
			sockaddr_in m_address;
		};
	}	
}