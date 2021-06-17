// [05/25/2021 MRB]

#pragma once

#include "Platform/Network/bcNetworkDefinitions.h"

namespace black_cat
{
	namespace platform
	{
		void bc_throw_network_exception();
		
		void bc_throw_network_exception(const bcCHAR* p_message);
		
		bcINT bc_cast_from_address_family(bc_socket_address p_address_family);

		bc_socket_address bc_cast_to_address_family(bcINT p_address_family);
		
		bcINT bc_cast_from_socket_type(bc_socket_type p_socket_type);

		bc_socket_type bc_cast_to_socket_type(bcINT p_socket_type);
		
		bcINT bc_cast_from_socket_protocol(bc_socket_protocol p_socket_protocol);

		bc_socket_protocol bc_cast_to_socket_protocol(bcINT p_socket_protocol);
	}	
}