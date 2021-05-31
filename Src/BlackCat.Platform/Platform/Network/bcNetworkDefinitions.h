// [05/23/2021 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Platform/bcExport.h"

namespace black_cat
{
	namespace platform
	{
		enum class bc_socket_address : bcUBYTE
		{
			inter_network,
			inter_network_v6
		};

		enum class bc_socket_type : bcUBYTE
		{
			data_gram,		// Connectionless, unreliable messages of a fixed maximum length. Messages might be lost or duplicated and might arrive out of order.
			stream			// Supports reliable, two-way, connection-based byte streams without the duplication of data and without preservation of boundaries.
		};

		enum class bc_socket_protocol : bcUBYTE
		{
			tcp,
			udp
		};

		BC_PLATFORMSHARED_DLL void bc_initialize_socket_library();

		BC_PLATFORMSHARED_DLL void bc_cleanup_socket_library();
	}	
}