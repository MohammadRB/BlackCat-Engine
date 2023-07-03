// [26/05/2021 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/Network/bcNetworkDefinitions.h"
#include "PlatformImp/bcUtility.h"
#include "PlatformImp/bcExport.h"
#include <WinSock2.h>

namespace black_cat
{
	namespace platform
	{
		WSADATA g_wsa_data;
		
		BC_PLATFORMSHARED_DLL void bc_initialize_socket_library()
		{
			const auto l_wsa_result = WSAStartup(MAKEWORD(2, 2), &g_wsa_data);
			if(l_wsa_result != 0)
			{
				bc_throw_network_exception();
			}
		}

		BC_PLATFORMSHARED_DLL void bc_cleanup_socket_library()
		{
			WSACleanup();
		}
	}	
}