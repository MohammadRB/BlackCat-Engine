// [05/25/2021 MRB]

#pragma once

#include "Platform/Network/bcNeworkDefinitions.h"

namespace black_cat
{
	namespace platform
	{
		void bc_throw_network_exception();
		
		void bc_throw_network_exception(const bcCHAR* p_message);
		
		bcINT bc_platform_cast(bc_socket_address p_address_family);
		
		bcINT bc_platform_cast(bc_socket_type p_socket_type);
		
		bcINT bc_platform_cast(bc_socket_protocol p_socket_type);
	}	
}